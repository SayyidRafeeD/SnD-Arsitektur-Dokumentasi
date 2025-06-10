#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Diperlukan untuk generate kode booking random

// --- DEFINISI TIPE DATA (STRUCT) ---

// Struct untuk menyimpan data penumpang
struct Penumpang {
    char nama[50];
    char ktp[20];
};

// Struct untuk merepresentasikan node dalam linked list penumpang
struct NodePenumpang {
    struct Penumpang data;
    struct NodePenumpang* next;
};

// Struct untuk merepresentasikan gerbong dengan layout kursi 2D
struct Gerbong {
    // Kita asumsikan 10 baris dan 4 kursi per baris (A, B, C, D)
    // 0 = kosong, 1 = terisi
    int kursi[10][4]; 
};

// Struct untuk merepresentasikan node stasiun (Vertex dalam Graph)
struct NodeStasiun {
    char nama[50];
    struct NodeStasiun* next;
};

// Struct untuk merepresentasikan node jadwal (Bagian dari Multi Linked List)
struct NodeJadwal {
    char namaKereta[50];
    struct NodeStasiun* stasiunAsal;
    struct NodeStasiun* stasiunTujuan;
    char waktuBerangkat[10];
    struct Gerbong gerbong;
    struct NodePenumpang* headPenumpang; // Pointer ke head linked list penumpang
    struct NodeJadwal* next;
};

// Struct untuk node Binary Search Tree (BST) untuk pencarian tiket cepat
struct NodeBST {
    char kodeBooking[10];
    struct Penumpang dataPenumpang;
    // Kita tidak bisa menyimpan detail rute/kursi di sini karena akan redundan
    // Data di sini adalah untuk verifikasi cepat
    struct NodeBST *kiri;
    struct NodeBST *kanan;
};


// --- POINTER GLOBAL ---
struct NodeJadwal* headJadwal = NULL;
struct NodeStasiun* headStasiun = NULL;
struct NodeBST* rootTiket = NULL;

// --- PROTOTYPE FUNGSI ---
void inisialisasiDataAwal();
struct NodeStasiun* cariStasiun(char* nama);
void tampilkanSemuaJadwal();
void menuPesanTiket();
void menuCekBooking();
struct NodeBST* insertTiket(struct NodeBST* root, struct NodeBST* nodeBaru);
struct NodeBST* cariTiketDiBST(struct NodeBST* root, char* kode);


// --- FUNGSI MANIPULASI DATA ---

struct NodeStasiun* buatStasiun(char* nama) {
    struct NodeStasiun* stasiunBaru = (struct NodeStasiun*)malloc(sizeof(struct NodeStasiun));
    strcpy(stasiunBaru->nama, nama);
    stasiunBaru->next = NULL;
    return stasiunBaru;
}

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

struct NodeStasiun* cariStasiun(char* nama) {
    struct NodeStasiun* temp = headStasiun;
    while (temp != NULL) {
        if (strcmp(temp->nama, nama) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

struct NodeJadwal* buatJadwal(char* namaKereta, char* asal, char* tujuan, char* waktu) {
    struct NodeJadwal* jadwalBaru = (struct NodeJadwal*)malloc(sizeof(struct NodeJadwal));
    
    strcpy(jadwalBaru->namaKereta, namaKereta);
    strcpy(jadwalBaru->waktuBerangkat, waktu);
    
    jadwalBaru->stasiunAsal = cariStasiun(asal);
    jadwalBaru->stasiunTujuan = cariStasiun(tujuan);
    
    if (jadwalBaru->stasiunAsal == NULL || jadwalBaru->stasiunTujuan == NULL) {
        printf("Error: Stasiun asal atau tujuan tidak ditemukan untuk jadwal %s.\n", namaKereta);
        free(jadwalBaru);
        return NULL;
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 4; j++) {
            jadwalBaru->gerbong.kursi[i][j] = 0;
        }
    }
    
    jadwalBaru->headPenumpang = NULL;
    jadwalBaru->next = NULL;
    
    return jadwalBaru;
}

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

// Fungsi untuk generate kode booking random
void generateKodeBooking(char* kode, int length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < length; i++) {
        int key = rand() % (sizeof(charset) - 1);
        kode[i] = charset[key];
    }
    kode[length] = '\0';
}


// --- FUNGSI TAMPILAN & MENU ---

void tampilkanSemuaJadwal() {
    struct NodeJadwal* temp = headJadwal;
    if (temp == NULL) {
        printf("Saat ini tidak ada jadwal yang tersedia.\n");
        return;
    }

    printf("\n--- DAFTAR JADWAL KERETA API ---\n");
    int nomor = 1;
    while (temp != NULL) {
        printf("%d. %-12s | %s -> %-15s | Berangkat: %s\n", 
               nomor, 
               temp->namaKereta, 
               temp->stasiunAsal->nama, 
               temp->stasiunTujuan->nama, 
               temp->waktuBerangkat);
        temp = temp->next;
        nomor++;
    }
    printf("----------------------------------\n\n");
}

void tampilkanKursi(struct NodeJadwal* jadwal) {
    printf("\n--- DENAH KURSI KERETA %s ---\n", jadwal->namaKereta);
    printf("  A B C D\n");
    for (int i = 0; i < 10; i++) {
        printf("%-2d", i + 1);
        for (int j = 0; j < 4; j++) {
            if (jadwal->gerbong.kursi[i][j] == 0) {
                printf("O "); // O untuk kosong (Available)
            } else {
                printf("X "); // X untuk terisi (Occupied)
            }
        }
        printf("\n");
    }
    printf("Keterangan: O = Kosong, X = Terisi\n\n");
}

void menuPesanTiket() {
    int pilihanJadwal, baris, kolom;
    char kolomChar;
    
    tampilkanSemuaJadwal();
    printf("Pilih nomor jadwal yang diinginkan: ");
    scanf("%d", &pilihanJadwal);

    struct NodeJadwal* jadwalPilihan = headJadwal;
    for (int i = 1; i < pilihanJadwal && jadwalPilihan != NULL; i++) {
        jadwalPilihan = jadwalPilihan->next;
    }

    if (jadwalPilihan == NULL) {
        printf("Nomor jadwal tidak valid.\n\n");
        return;
    }

    tampilkanKursi(jadwalPilihan);
    printf("Pilih kursi (contoh: 1A, 10C): ");
    scanf("%d%c", &baris, &kolomChar);
    
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

    jadwalPilihan->gerbong.kursi[baris][kolom] = 1;

    struct NodePenumpang* penumpangBaru = (struct NodePenumpang*)malloc(sizeof(struct NodePenumpang));
    penumpangBaru->data = p;
    penumpangBaru->next = jadwalPilihan->headPenumpang;
    jadwalPilihan->headPenumpang = penumpangBaru;
    
    struct NodeBST* tiketBaru = (struct NodeBST*)malloc(sizeof(struct NodeBST));
    tiketBaru->dataPenumpang = p;
    generateKodeBooking(tiketBaru->kodeBooking, 6);
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

struct NodeBST* insertTiket(struct NodeBST* root, struct NodeBST* nodeBaru) {
    if (root == NULL) return nodeBaru;
    if (strcmp(nodeBaru->kodeBooking, root->kodeBooking) < 0) {
        root->kiri = insertTiket(root->kiri, nodeBaru);
    } else {
        root->kanan = insertTiket(root->kanan, nodeBaru);
    }
    return root;
}

// Fungsi untuk mencari tiket di BST (rekursif)
struct NodeBST* cariTiketDiBST(struct NodeBST* root, char* kode) {
    // Base case: root adalah NULL atau kode ditemukan di root
    if (root == NULL || strcmp(root->kodeBooking, kode) == 0) {
        return root;
    }
    
    // Jika kode yang dicari lebih kecil, cari di sub-pohon kiri
    if (strcmp(kode, root->kodeBooking) < 0) {
        return cariTiketDiBST(root->kiri, kode);
    }
    
    // Jika kode yang dicari lebih besar, cari di sub-pohon kanan
    return cariTiketDiBST(root->kanan, kode);
}

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
        printf("Status         : Terverifikasi\n");
        printf("------------------------\n\n");
    } else {
        printf("\nKode Booking '%s' tidak ditemukan atau tidak valid.\n\n", kode);
    }
}


// --- FUNGSI UTAMA & INISIALISASI ---

int main() {
    srand(time(NULL));
    inisialisasiDataAwal();
    
    int pilihan = 0;
    while (pilihan != 4) {
        printf("=============================\n");
        printf("   SISTEM RESERVASI KERETA   \n");
        printf("=============================\n");
        printf("1. Lihat Semua Jadwal\n");
        printf("2. Pesan Tiket\n");
        printf("3. Cek Kode Booking\n");
        printf("4. Keluar\n");
        printf("-----------------------------\n");
        printf("Pilihan Anda: ");
        scanf("%d", &pilihan);
        
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
                printf("\nTerima kasih telah menggunakan sistem kami!\n");
                break;
            default:
                printf("\nPilihan tidak valid. Silakan coba lagi.\n\n");
                break;
        }
    }
    
    return 0;
}

void inisialisasiDataAwal() {
    tambahStasiun("Gambir");
    tambahStasiun("Bandung");
    tambahStasiun("Yogyakarta");
    tambahStasiun("Surabaya Gubeng");
    
    tambahJadwal("Argo Wilis", "Bandung", "Surabaya Gubeng", "08:00");
    tambahJadwal("Argo Bromo", "Gambir", "Surabaya Gubeng", "20:00");
    tambahJadwal("Taksaka", "Gambir", "Yogyakarta", "09:00");
}
