#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- DEFINISI TIPE DATA (STRUCT) ---

// Data penumpang
struct Penumpang {
    char nama[50];
    char ktp[20];
};

// Node dalam linked list penumpang
struct NodePenumpang {
    struct Penumpang data;
    struct NodePenumpang* next;
};

// Node dalam antrian daftar tunggu (Queue)
struct NodeAntrian {
    struct Penumpang data;
    struct NodeAntrian* next;
};

// Queue untuk mengelola daftar tunggu
struct Queue {
    struct NodeAntrian *front, *rear;
    int count; // Menghitung jumlah penumpang dalam antrian
};

// Gerbong dengan layout kursi 2D
struct Gerbong {
    // Layout kursi 10 baris x 4 kolom (A,B,C,D)
    // 0 = kosong, 1 = terisi
    int kursi[10][4]; 
    int kapasitas; // Melacak jumlah kursi yang sudah terisi
};

// Node stasiun (Vertex dalam Graph)
struct NodeStasiun {
    char nama[50];
    struct NodeStasiun* next;
};

// Node jadwal (Multi Linked List)
struct NodeJadwal {
    char namaKereta[50];
    struct NodeStasiun* stasiunAsal;
    struct NodeStasiun* stasiunTujuan;
    char waktuBerangkat[10];
    struct Gerbong gerbong;
    struct NodePenumpang* headPenumpang; // Head dari linked list penumpang
    struct Queue daftarTunggu;          // Queue untuk daftar tunggu
    struct NodeJadwal* next;
};

// Node BST untuk pencarian tiket
struct NodeBST {
    char kodeBooking[10];
    struct Penumpang dataPenumpang;
    struct NodeJadwal* jadwalTerkait; // Pointer ke jadwal tiket
    int barisKursi;                  // Menyimpan posisi baris kursi
    int kolomKursi;                  // Menyimpan posisi kolom kursi
    struct NodeBST *kiri;
    struct NodeBST *kanan;
};


// --- POINTER GLOBAL ---
struct NodeJadwal* headJadwal = NULL;
struct NodeStasiun* headStasiun = NULL;
struct NodeBST* rootTiket = NULL; // Root dari BST Tiket

// --- PROTOTYPE FUNGSI ---
void inisialisasiDataAwal();
struct NodeStasiun* cariStasiun(char* nama);
void tampilkanSemuaJadwal();
void menuPesanTiket();
void menuCekBooking();
void menuBatalTiket();
void menuLihatDaftarTunggu();
struct NodeBST* insertTiket(struct NodeBST* root, struct NodeBST* nodeBaru);
struct NodeBST* cariTiketDiBST(struct NodeBST* root, char* kode);
struct NodeBST* hapusTiketDariBST(struct NodeBST* root, char* kode);


// --- MANIPULASI STASIUN & JADWAL ---

// Membuat node stasiun baru
struct NodeStasiun* buatStasiun(char* nama) {
    struct NodeStasiun* stasiunBaru = (struct NodeStasiun*)malloc(sizeof(struct NodeStasiun));
    strcpy(stasiunBaru->nama, nama);
    stasiunBaru->next = NULL;
    return stasiunBaru;
}

// Menambahkan stasiun ke dalam linked list stasiun
void tambahStasiun(char* nama) {
    struct NodeStasiun* stasiunBaru = buatStasiun(nama);
    if (headStasiun == NULL) {
        headStasiun = stasiunBaru;
    } else {
        struct NodeStasiun* temp = headStasiun;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = stasiunBaru;
    }
}

// Mencari stasiun berdasarkan nama
struct NodeStasiun* cariStasiun(char* nama) {
    struct NodeStasiun* temp = headStasiun;
    while (temp != NULL) {
        if (strcmp(temp->nama, nama) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL; // Return NULL jika tidak ditemukan
}

// Inisialisasi queue
void initQueue(struct Queue *q) {
    q->front = q->rear = NULL;
    q->count = 0;
}

// Membuat jadwal kereta baru
struct NodeJadwal* buatJadwal(char* namaKereta, char* asal, char* tujuan, char* waktu) {
    struct NodeJadwal* jadwalBaru = (struct NodeJadwal*)malloc(sizeof(struct NodeJadwal));
    
    strcpy(jadwalBaru->namaKereta, namaKereta);
    strcpy(jadwalBaru->waktuBerangkat, waktu);
    
    jadwalBaru->stasiunAsal = cariStasiun(asal);
    jadwalBaru->stasiunTujuan = cariStasiun(tujuan);
    
    if (jadwalBaru->stasiunAsal == NULL || jadwalBaru->stasiunTujuan == NULL) {
        printf("Error: Stasiun asal atau tujuan tidak valid untuk jadwal %s.\n", namaKereta);
        free(jadwalBaru);
        return NULL;
    }

    // Inisialisasi gerbong dan kapasitas awal
    jadwalBaru->gerbong.kapasitas = 0; 
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 4; j++) {
            jadwalBaru->gerbong.kursi[i][j] = 0; // Set semua kursi kosong
        }
    }
    
    jadwalBaru->headPenumpang = NULL;
    initQueue(&jadwalBaru->daftarTunggu); // Inisialisasi daftar tunggu
    jadwalBaru->next = NULL;
    
    return jadwalBaru;
}

// Menambahkan jadwal ke dalam linked list jadwal
void tambahJadwal(char* namaKereta, char* asal, char* tujuan, char* waktu) {
    struct NodeJadwal* jadwalBaru = buatJadwal(namaKereta, asal, tujuan, waktu);
    if (jadwalBaru == NULL) return;

    if (headJadwal == NULL) {
        headJadwal = jadwalBaru;
    } else {
        struct NodeJadwal* temp = headJadwal;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = jadwalBaru;
    }
}

// --- QUEUE (DAFTAR TUNGGU) ---

// Cek apakah antrian kosong
int isAntrianKosong(struct Queue *q) {
    return (q->front == NULL);
}

// Menambahkan penumpang ke antrian (Enqueue)
void enqueueAntrian(struct Queue *q, struct Penumpang p) {
    struct NodeAntrian* nodeBaru = (struct NodeAntrian*)malloc(sizeof(struct NodeAntrian));
    nodeBaru->data = p;
    nodeBaru->next = NULL;
    q->count++;

    if (q->rear == NULL) { // Jika antrian kosong
        q->front = q->rear = nodeBaru;
        printf("Anda telah berhasil ditambahkan ke dalam daftar tunggu.\n");
        return;
    }

    q->rear->next = nodeBaru;
    q->rear = nodeBaru;
    printf("Anda telah berhasil ditambahkan ke dalam daftar tunggu.\n");
}

// Mengambil penumpang dari antrian (Dequeue)
struct Penumpang dequeueAntrian(struct Queue *q) {
    if (isAntrianKosong(q)) {
        struct Penumpang p_kosong = {"", ""}; // Return data kosong jika queue kosong
        return p_kosong;
    }

    struct NodeAntrian* temp = q->front;
    struct Penumpang dataDikeluarkan = temp->data;

    q->front = q->front->next;
    q->count--;

    if (q->front == NULL) { // Jika setelah dequeue antrian jadi kosong
        q->rear = NULL;
    }

    free(temp);
    return dataDikeluarkan;
}

// --- PEMBATALAN TIKET & MANIPULASI DATA ---

// Generate kode booking random unik
void generateKodeBooking(char* kode, int length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    do {
        for (int i = 0; i < length; i++) {
            int key = rand() % (sizeof(charset) - 1);
            kode[i] = charset[key];
        }
        kode[length] = '\0';
    } while (cariTiketDiBST(rootTiket, kode) != NULL); // Pastikan kode unik
}

// Hapus penumpang dari linked list penumpang di jadwal tertentu
int hapusPenumpangDariJadwal(struct NodeJadwal* jadwal, const char* ktp) {
    struct NodePenumpang *temp = jadwal->headPenumpang, *prev = NULL;

    // Jika node pertama adalah yang dicari
    if (temp != NULL && strcmp(temp->data.ktp, ktp) == 0) {
        jadwal->headPenumpang = temp->next;
        free(temp);
        return 1;
    }

    // Cari node yang akan dihapus
    while (temp != NULL && strcmp(temp->data.ktp, ktp) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) return 0; 

    prev->next = temp->next;
    free(temp);
    return 1;
}

// Helper untuk mencari node dengan nilai minimum di BST (untuk proses hapus)
struct NodeBST* findMin(struct NodeBST* root) {
    while (root->kiri != NULL) {
        root = root->kiri;
    }
    return root;
}

// Menghapus tiket dari BST (rekursif)
struct NodeBST* hapusTiketDariBST(struct NodeBST* root, char* kode) {
    if (root == NULL) return root;

    // Cari node yang akan dihapus
    if (strcmp(kode, root->kodeBooking) < 0) {
        root->kiri = hapusTiketDariBST(root->kiri, kode);
    } else if (strcmp(kode, root->kodeBooking) > 0) {
        root->kanan = hapusTiketDariBST(root->kanan, kode);
    } else {
        // Node ditemukan
        // Kasus 1: Node tidak punya anak atau punya 1 anak
        if (root->kiri == NULL) {
            struct NodeBST* temp = root->kanan;
            free(root);
            return temp;
        } else if (root->kanan == NULL) {
            struct NodeBST* temp = root->kiri;
            free(root);
            return temp;
        }

        // Kasus 2: Node punya 2 anak
        struct NodeBST* temp = findMin(root->kanan); // Cari inorder successor
        
        // Salin data inorder successor ke node ini
        strcpy(root->kodeBooking, temp->kodeBooking);
        root->dataPenumpang = temp->dataPenumpang;
        root->jadwalTerkait = temp->jadwalTerkait;
        root->barisKursi = temp->barisKursi;
        root->kolomKursi = temp->kolomKursi;

        // Hapus inorder successor
        root->kanan = hapusTiketDariBST(root->kanan, temp->kodeBooking);
    }
    return root;
}

// --- TAMPILAN & MENU UTAMA ---

// Menampilkan semua jadwal yang tersedia
void tampilkanSemuaJadwal() {
    struct NodeJadwal* temp = headJadwal;
    if (temp == NULL) {
        printf("Saat ini tidak ada jadwal yang tersedia.\n");
        return;
    }

    printf("\n--- DAFTAR JADWAL KERETA API ---\n");
    int nomor = 1;
    while (temp != NULL) {
        printf("%d. %-12s | %s -> %-15s | Berangkat: %s | Kursi: %d/40 | Tunggu: %d\n", 
               nomor, 
               temp->namaKereta, 
               temp->stasiunAsal->nama, 
               temp->stasiunTujuan->nama, 
               temp->waktuBerangkat,
               temp->gerbong.kapasitas,
               temp->daftarTunggu.count);
        temp = temp->next;
        nomor++;
    }
    printf("----------------------------------\n\n");
}

// Menampilkan denah kursi kereta
void tampilkanKursi(struct NodeJadwal* jadwal) {
    printf("\n--- DENAH KURSI KERETA %s ---\n", jadwal->namaKereta);
    printf("    A B   C D\n");
    for (int i = 0; i < 10; i++) {
        printf("%-3d ", i + 1);
        for (int j = 0; j < 4; j++) {
            if (jadwal->gerbong.kursi[i][j] == 0) {
                printf("O "); // O = kosong (Available)
            } else {
                printf("X "); // X = terisi (Occupied)
            }
            if (j == 1) printf("  "); // Spasi pemisah
        }
        printf("\n");
    }
    printf("Keterangan: O = Kosong, X = Terisi\n\n");
}

// Menu pemesanan tiket
void menuPesanTiket() {
    int pilihanJadwal, baris, kolom;
    char kolomChar;
    
    tampilkanSemuaJadwal();
    printf("Pilih nomor jadwal yang diinginkan: ");
    if (scanf("%d", &pilihanJadwal) != 1) {
        printf("Input tidak valid.\n");
        while(getchar()!='\n'); // Clear buffer
        return;
    }

    struct NodeJadwal* jadwalPilihan = headJadwal;
    for (int i = 1; i < pilihanJadwal && jadwalPilihan != NULL; i++) {
        jadwalPilihan = jadwalPilihan->next;
    }

    if (jadwalPilihan == NULL) {
        printf("Nomor jadwal tidak valid.\n\n");
        return;
    }

    // Cek jika kereta sudah penuh, tawarkan daftar tunggu
    if (jadwalPilihan->gerbong.kapasitas >= 40) {
        char pilihanTunggu;
        printf("Maaf, kereta %s sudah penuh.\n", jadwalPilihan->namaKereta);
        printf("Apakah Anda ingin masuk ke daftar tunggu? (y/n): ");
        scanf(" %c", &pilihanTunggu);
        if (pilihanTunggu == 'y' || pilihanTunggu == 'Y') {
            struct Penumpang p;
            printf("Masukkan nama Anda: ");
            scanf(" %[^\n]", p.nama);
            printf("Masukkan nomor KTP: ");
            scanf("%s", p.ktp);
            enqueueAntrian(&jadwalPilihan->daftarTunggu, p);
        }
        return;
    }

    tampilkanKursi(jadwalPilihan);
    printf("Pilih kursi (contoh: 1A, 10C): ");
    if (scanf("%d%c", &baris, &kolomChar) != 2) {
        printf("Format input tidak valid.\n\n");
        while(getchar() != '\n');
        return;
    }
    
    if (kolomChar >= 'a' && kolomChar <= 'd') kolom = kolomChar - 'a';
    else if (kolomChar >= 'A' && kolomChar <= 'D') kolom = kolomChar - 'A';
    else {
        printf("Format kolom tidak valid.\n\n");
        return;
    }
    baris--;

    if (baris < 0 || baris >= 10 || kolom < 0 || kolom >= 4) {
        printf("Nomor baris atau kolom tidak valid.\n\n");
        return;
    }

    if (jadwalPilihan->gerbong.kursi[baris][kolom] == 1) {
        printf("Maaf, kursi %d%c sudah terisi. Silakan pilih kursi lain.\n\n", baris + 1, kolom + 'A');
        return;
    }
    
    struct Penumpang p;
    printf("Masukkan nama Anda: ");
    scanf(" %[^\n]", p.nama);
    printf("Masukkan nomor KTP: ");
    scanf("%s", p.ktp);

    // Update status kursi dan kapasitas
    jadwalPilihan->gerbong.kursi[baris][kolom] = 1;
    jadwalPilihan->gerbong.kapasitas++;

    // Tambahkan penumpang ke linked list penumpang
    struct NodePenumpang* penumpangBaru = (struct NodePenumpang*)malloc(sizeof(struct NodePenumpang));
    penumpangBaru->data = p;
    penumpangBaru->next = jadwalPilihan->headPenumpang;
    jadwalPilihan->headPenumpang = penumpangBaru;
    
    // Buat tiket dan masukkan ke BST
    struct NodeBST* tiketBaru = (struct NodeBST*)malloc(sizeof(struct NodeBST));
    tiketBaru->dataPenumpang = p;
    generateKodeBooking(tiketBaru->kodeBooking, 6);
    tiketBaru->jadwalTerkait = jadwalPilihan; 
    tiketBaru->barisKursi = baris;
    tiketBaru->kolomKursi = kolom;
    tiketBaru->kiri = tiketBaru->kanan = NULL;
    rootTiket = insertTiket(rootTiket, tiketBaru);

    printf("\n--- PEMESANAN BERHASIL ---\n");
    printf("Kode Booking   : %s\n", tiketBaru->kodeBooking);
    printf("Nama Penumpang : %s\n", p.nama);
    printf("Kereta         : %s\n", jadwalPilihan->namaKereta);
    printf("Rute           : %s -> %s\n", jadwalPilihan->stasiunAsal->nama, jadwalPilihan->stasiunTujuan->nama);
    printf("Kursi          : %d%c\n", baris + 1, kolom + 'A');
    printf("--------------------------\n\n");
}

// Menambahkan tiket ke BST (rekursif)
struct NodeBST* insertTiket(struct NodeBST* root, struct NodeBST* nodeBaru) {
    if (root == NULL) return nodeBaru;
    // Bandingkan kode booking untuk menentukan posisi
    if (strcmp(nodeBaru->kodeBooking, root->kodeBooking) < 0) {
        root->kiri = insertTiket(root->kiri, nodeBaru);
    } else {
        root->kanan = insertTiket(root->kanan, nodeBaru);
    }
    return root;
}

// Mencari tiket di BST (rekursif)
struct NodeBST* cariTiketDiBST(struct NodeBST* root, char* kode) {
    if (root == NULL || strcmp(root->kodeBooking, kode) == 0) {
        return root;
    }
    if (strcmp(kode, root->kodeBooking) < 0) {
        return cariTiketDiBST(root->kiri, kode);
    }
    return cariTiketDiBST(root->kanan, kode);
}

// Menu pengecekan kode booking
void menuCekBooking() {
    char kode[10];
    printf("\n--- FITUR CEK BOOKING ---\n");
    printf("Masukkan Kode Booking Anda: ");
    scanf("%s", kode);

    struct NodeBST* tiket = cariTiketDiBST(rootTiket, kode);

    if (tiket != NULL) {
        printf("\n--- TIKET DITEMUKAN ---\n");
        printf("Kode Booking   : %s\n", tiket->kodeBooking);
        printf("Nama Penumpang : %s\n", tiket->dataPenumpang.nama);
        printf("Nomor KTP      : %s\n", tiket->dataPenumpang.ktp);
        printf("Kereta         : %s\n", tiket->jadwalTerkait->namaKereta);
        printf("Kursi          : %d%c\n", tiket->barisKursi + 1, tiket->kolomKursi + 'A');
        printf("Status         : Terverifikasi\n");
        printf("------------------------\n\n");
    } else {
        printf("\nKode Booking '%s' tidak ditemukan atau tidak valid.\n\n", kode);
    }
}

// Menu pembatalan tiket
void menuBatalTiket() {
    char kode[10];
    printf("\n--- FITUR PEMBATALAN TIKET ---\n");
    printf("Masukkan Kode Booking yang ingin dibatalkan: ");
    scanf("%s", kode);

    struct NodeBST* tiket = cariTiketDiBST(rootTiket, kode);

    if (tiket != NULL) {
        struct NodeJadwal* jadwal = tiket->jadwalTerkait;
        int baris = tiket->barisKursi;
        int kolom = tiket->kolomKursi;

        // 1. Kosongkan kursi dan kurangi kapasitas
        jadwal->gerbong.kursi[baris][kolom] = 0;
        jadwal->gerbong.kapasitas--;
        
        // 2. Hapus penumpang dari linked list
        hapusPenumpangDariJadwal(jadwal, tiket->dataPenumpang.ktp);

        printf("\n--- PEMBATALAN BERHASIL ---\n");
        printf("Tiket dengan kode %s untuk penumpang %s telah dibatalkan.\n", tiket->kodeBooking, tiket->dataPenumpang.nama);
        printf("Kursi %d%c pada kereta %s sekarang tersedia.\n", baris + 1, kolom + 'A', jadwal->namaKereta);
        
        // 3. Hapus tiket dari BST (setelah data lain digunakan)
        char ktp_penumpang_dihapus[20];
        strcpy(ktp_penumpang_dihapus, tiket->dataPenumpang.ktp); // Simpan KTP sebelum tiket dihapus
        rootTiket = hapusTiketDariBST(rootTiket, kode);
        
        // 4. Proses daftar tunggu jika ada
        if (!isAntrianKosong(&jadwal->daftarTunggu)) {
            printf("\nAda penumpang di daftar tunggu. Memproses...\n");
            struct Penumpang p_baru = dequeueAntrian(&jadwal->daftarTunggu);
            
            // Tempatkan penumpang baru ke kursi yang kosong
            jadwal->gerbong.kursi[baris][kolom] = 1;
            jadwal->gerbong.kapasitas++;
            
            // Tambahkan ke list penumpang
            struct NodePenumpang* pNodeBaru = (struct NodePenumpang*)malloc(sizeof(struct NodePenumpang));
            pNodeBaru->data = p_baru;
            pNodeBaru->next = jadwal->headPenumpang;
            jadwal->headPenumpang = pNodeBaru;
            
            // Buat tiket baru untuk penumpang dari daftar tunggu
            struct NodeBST* tiketBaru = (struct NodeBST*)malloc(sizeof(struct NodeBST));
            tiketBaru->dataPenumpang = p_baru;
            generateKodeBooking(tiketBaru->kodeBooking, 6);
            tiketBaru->jadwalTerkait = jadwal;
            tiketBaru->barisKursi = baris;
            tiketBaru->kolomKursi = kolom;
            tiketBaru->kiri = tiketBaru->kanan = NULL;
            rootTiket = insertTiket(rootTiket, tiketBaru);

            printf("\n--- PROSES DAFTAR TUNGGU BERHASIL ---\n");
            printf("Penumpang %s telah dipindahkan dari daftar tunggu.\n", p_baru.nama);
            printf("Kode Booking Baru: %s\n", tiketBaru->kodeBooking);
            printf("Kursi            : %d%c\n", baris + 1, kolom + 'A');
            printf("---------------------------------------\n\n");
        } else {
             printf("---------------------------\n\n");
        }

    } else {
        printf("\nKode Booking '%s' tidak ditemukan atau tidak valid.\n\n", kode);
    }
}

// Melihat isi daftar tunggu
void menuLihatDaftarTunggu() {
    tampilkanSemuaJadwal();
    int pilihanJadwal;
    printf("Pilih nomor jadwal untuk melihat daftar tunggu: ");
    if (scanf("%d", &pilihanJadwal) != 1) {
        printf("Input tidak valid.\n");
        while(getchar()!='\n'); // Clear buffer
        return;
    }

    struct NodeJadwal* jadwalPilihan = headJadwal;
    for (int i = 1; i < pilihanJadwal && jadwalPilihan != NULL; i++) {
        jadwalPilihan = jadwalPilihan->next;
    }

    if (jadwalPilihan == NULL) {
        printf("Nomor jadwal tidak valid.\n\n");
        return;
    }

    printf("\n--- DAFTAR TUNGGU KERETA %s ---\n", jadwalPilihan->namaKereta);
    if (isAntrianKosong(&jadwalPilihan->daftarTunggu)) {
        printf("Daftar tunggu kosong.\n");
    } else {
        struct NodeAntrian* temp = jadwalPilihan->daftarTunggu.front;
        int i = 1;
        while(temp != NULL) {
            printf("%d. Nama: %-20s | KTP: %s\n", i, temp->data.nama, temp->data.ktp);
            temp = temp->next;
            i++;
        }
    }
    printf("----------------------------------\n\n");
}

int main() {
    srand(time(NULL)); // Inisialisasi seed untuk angka random
    inisialisasiDataAwal();
    
    int pilihan = 0;
    while (pilihan != 6) {
        printf("=============================\n");
        printf("   SISTEM RESERVASI KERETA   \n");
        printf("=============================\n");
        printf("1. Lihat Semua Jadwal\n");
        printf("2. Pesan Tiket\n");
        printf("3. Cek Kode Booking\n");
        printf("4. Batalkan Tiket\n");
        printf("5. Lihat Daftar Tunggu\n");
        printf("6. Keluar\n");
        printf("-----------------------------\n");
        printf("Pilihan Anda: ");
        
        // Handling input non-numerik
        if (scanf("%d", &pilihan) != 1) {
            printf("\nInput tidak valid. Harap masukkan angka.\n\n");
            while(getchar() != '\n'); // Membersihkan buffer input
            continue;
        }
        
        switch (pilihan) {
            case 1:
                tampilkanSemuaJadwal();
                break;
            case 2:
                menuPesanTiket();
                break;
            case 3:
                menuCekBooking();
                break;
            case 4:
                menuBatalTiket();
                break;
            case 5:
                menuLihatDaftarTunggu();
                break;
            case 6:
                printf("\nTerima kasih telah menggunakan sistem kami!\n");
                break;
            default:
                printf("\nPilihan tidak valid. Silakan coba lagi.\n\n");
                break;
        }
    }
    
    return 0;
}

// Inisialisasi data awal (stasiun dan jadwal)
void inisialisasiDataAwal() {
    tambahStasiun("Gambir");
    tambahStasiun("Bandung");
    tambahStasiun("Yogyakarta");
    tambahStasiun("Surabaya Gubeng");
    tambahStasiun("Malang");
    
    tambahJadwal("Argo Wilis", "Bandung", "Surabaya Gubeng", "08:00");
    tambahJadwal("Argo Bromo", "Gambir", "Surabaya Gubeng", "20:00");
    tambahJadwal("Taksaka", "Gambir", "Yogyakarta", "09:00");
    tambahJadwal("Gajayana", "Gambir", "Malang", "18:00");
}
