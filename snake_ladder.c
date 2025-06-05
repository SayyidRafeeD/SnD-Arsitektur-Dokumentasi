#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PLAYERS 4
#define MAX_NAME_LENGTH 50
#define MAX_HISTORY 100

// Struktur untuk stack riwayat posisi (untuk fitur undo)
typedef struct {
    int positions[MAX_HISTORY];
    int top;
} PositionStack;

// Struktur untuk pemain
typedef struct {
    char name[MAX_NAME_LENGTH];
    int position;
    PositionStack history;
} Player;

// Struktur untuk ular dan tangga
typedef struct {
    int start;
    int end;
} SnakeOrLadder;

// Node untuk queue pemain
typedef struct PlayerNode {
    Player player;
    struct PlayerNode* next;
} PlayerNode;

// Struktur queue untuk mengelola giliran pemain
typedef struct {
    PlayerNode* front;
    PlayerNode* rear;
    int count;
} PlayerQueue;

// === FUNGSI STACK (untuk riwayat posisi) ===

// Inisialisasi stack
void initStack(PositionStack* stack) {
    stack->top = -1;
}

// Push posisi ke stack
void pushPosition(PositionStack* stack, int position) {
    if (stack->top < MAX_HISTORY - 1) {
        stack->top++;
        stack->positions[stack->top] = position;
    }
}

// Pop posisi dari stack (untuk undo)
int popPosition(PositionStack* stack) {
    if (stack->top >= 0) {
        int position = stack->positions[stack->top];
        stack->top--;
        return position;
    }
    return -1; // Stack kosong
}

// Cek apakah stack kosong
int isStackEmpty(PositionStack* stack) {
    return stack->top == -1;
}

// === FUNGSI QUEUE (untuk giliran pemain) ===

// Inisialisasi queue
void initQueue(PlayerQueue* queue) {
    queue->front = NULL;
    queue->rear = NULL;
    queue->count = 0;
}

// Enqueue - menambah pemain ke antrian
int enqueue(PlayerQueue* queue, Player player) {
    if (queue->count >= MAX_PLAYERS) {
        printf("Error: Maksimal %d pemain!\n", MAX_PLAYERS);
        return 0;
    }
    
    PlayerNode* newNode = (PlayerNode*)malloc(sizeof(PlayerNode));
    if (newNode == NULL) {
        printf("Error: Gagal mengalokasi memori!\n");
        return 0;
    }
    
    newNode->player = player;
    newNode->next = NULL;
    
    if (queue->rear == NULL) {
        // Queue kosong
        queue->front = queue->rear = newNode;
    } else {
        // Tambah di akhir queue
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    
    queue->count++;
    return 1;
}

// Dequeue - mengambil pemain dari depan antrian
Player* dequeue(PlayerQueue* queue) {
    if (queue->front == NULL) {
        return NULL; // Queue kosong
    }
    
    PlayerNode* temp = queue->front;
    Player* player = &(temp->player);
    
    queue->front = queue->front->next;
    
    if (queue->front == NULL) {
        queue->rear = NULL; // Queue menjadi kosong
    }
    
    queue->count--;
    
    // Note: Dalam implementasi ini, kita mengembalikan pointer ke player
    // yang akan segera di-free. Untuk penggunaan sebenarnya, Anda perlu
    // menyalin data player sebelum membebaskan node
    
    return player;
}

// Cek apakah queue kosong
int isQueueEmpty(PlayerQueue* queue) {
    return queue->front == NULL;
}

// Mendapatkan jumlah pemain dalam queue
int getPlayerCount(PlayerQueue* queue) {
    return queue->count;
}

// === FUNGSI HELPER ===

// Inisialisasi pemain baru
Player createPlayer(const char* name) {
    Player player;
    strncpy(player.name, name, MAX_NAME_LENGTH - 1);
    player.name[MAX_NAME_LENGTH - 1] = '\0'; // Pastikan null-terminated
    player.position = 1; // Mulai dari posisi 1
    initStack(&player.history);
    pushPosition(&player.history, 1); // Simpan posisi awal
    return player;
}

// Fungsi untuk membebaskan memori queue
void freeQueue(PlayerQueue* queue) {
    while (!isQueueEmpty(queue)) {
        PlayerNode* temp = queue->front;
        queue->front = queue->front->next;
        free(temp);
    }
    queue->rear = NULL;
    queue->count = 0;
}

// === FUNGSI INISIALISASI ULAR DAN TANGGA ===

// Fungsi untuk menginisialisasi ular dengan konfigurasi standar
void inisialisasiUlar(SnakeOrLadder ular[], int* jumlahUlar) {
    SnakeOrLadder konfigurasiUlar[] = {
        {16, 6},   // Ular dari 16 turun ke 6
        {47, 26},  // Ular dari 47 turun ke 26
        {49, 11},  // Ular dari 49 turun ke 11
        {56, 53},  // Ular dari 56 turun ke 53
        {62, 19},  // Ular dari 62 turun ke 19
        {64, 60},  // Ular dari 64 turun ke 60
        {87, 24},  // Ular dari 87 turun ke 24
        {93, 73},  // Ular dari 93 turun ke 73
        {95, 75},  // Ular dari 95 turun ke 75
        {98, 78}   // Ular dari 98 turun ke 78
    };
    
    *jumlahUlar = sizeof(konfigurasiUlar) / sizeof(konfigurasiUlar[0]);
    
    for (int i = 0; i < *jumlahUlar; i++) {
        ular[i] = konfigurasiUlar[i];
    }
    
    printf("=== ULAR TELAH DIINISIALISASI ===\n");
    for (int i = 0; i < *jumlahUlar; i++) {
        printf("Ular %d: Posisi %d -> %d (turun %d kotak)\n", 
               i + 1, ular[i].start, ular[i].end, 
               ular[i].start - ular[i].end);
    }
    printf("Total ular: %d\n\n", *jumlahUlar);
}

// Fungsi untuk menginisialisasi tangga dengan konfigurasi standar
void inisialisasiTangga(SnakeOrLadder tangga[], int* jumlahTangga) {
    SnakeOrLadder konfigurasiTangga[] = {
        {1, 38},   // Tangga dari 1 naik ke 38
        {4, 14},   // Tangga dari 4 naik ke 14
        {9, 31},   // Tangga dari 9 naik ke 31
        {21, 42},  // Tangga dari 21 naik ke 42
        {28, 84},  // Tangga dari 28 naik ke 84
        {36, 44},  // Tangga dari 36 naik ke 44
        {51, 67},  // Tangga dari 51 naik ke 67
        {71, 91},  // Tangga dari 71 naik ke 91
        {80, 100}  // Tangga dari 80 naik ke 100
    };
    
    *jumlahTangga = sizeof(konfigurasiTangga) / sizeof(konfigurasiTangga[0]);
    
    for (int i = 0; i < *jumlahTangga; i++) {
        tangga[i] = konfigurasiTangga[i];
    }
    
    printf("=== TANGGA TELAH DIINISIALISASI ===\n");
    for (int i = 0; i < *jumlahTangga; i++) {
        printf("Tangga %d: Posisi %d -> %d (naik %d kotak)\n", 
               i + 1, tangga[i].start, tangga[i].end, 
               tangga[i].end - tangga[i].start);
    }
    printf("Total tangga: %d\n\n", *jumlahTangga);
}

// === FUNGSI GAMEPLAY ===

// Fungsi untuk melempar dadu (acak 1-6)
int lemparDadu() {
    return (rand() % 6) + 1;
}

// Fungsi untuk cek apakah posisi ada ular
int cekUlar(int posisi, SnakeOrLadder ular[], int jumlahUlar) {
    for (int i = 0; i < jumlahUlar; i++) {
        if (ular[i].start == posisi) {
            return ular[i].end; // Kembalikan posisi tujuan ular
        }
    }
    return -1; // Tidak ada ular di posisi ini
}

// Fungsi untuk cek apakah posisi ada tangga
int cekTangga(int posisi, SnakeOrLadder tangga[], int jumlahTangga) {
    for (int i = 0; i < jumlahTangga; i++) {
        if (tangga[i].start == posisi) {
            return tangga[i].end; // Kembalikan posisi tujuan tangga
        }
    }
    return -1; // Tidak ada tangga di posisi ini
}

// Fungsi untuk memindahkan pemain
int pindahkanPemain(Player* pemain, int hasilDadu, SnakeOrLadder ular[], int jumlahUlar, 
                   SnakeOrLadder tangga[], int jumlahTangga) {
    
    // Simpan posisi lama ke stack untuk fitur undo
    pushPosition(&pemain->history, pemain->position);
    
    int posisiLama = pemain->position;
    int posisiBaru = posisiLama + hasilDadu;
    
    printf("\n=== GILIRAN %s ===\n", pemain->name);
    printf("Posisi awal: %d\n", posisiLama);
    printf("Hasil dadu: %d\n", hasilDadu);
    
    // Cek apakah melebihi kotak 100
    if (posisiBaru > 100) {
        printf("Tidak bisa bergerak! Dadu terlalu besar (akan melewati kotak 100)\n");
        printf("Posisi tetap: %d\n", posisiLama);
        printf("==============================\n");
        return 0; // Tidak bergerak
    }
    
    printf("Bergerak ke posisi: %d\n", posisiBaru);
    pemain->position = posisiBaru;
    
    // Cek apakah menang
    if (posisiBaru == 100) {
        printf("🎉 SELAMAT! %s MENANG! 🎉\n", pemain->name);
        printf("==============================\n");
        return 1; // Menang
    }
    
    // Cek ular
    int posisiUlar = cekUlar(posisiBaru, ular, jumlahUlar);
    if (posisiUlar != -1) {
        printf("🐍 OH NO! Kena ular di posisi %d!\n", posisiBaru);
        printf("Turun ke posisi: %d (turun %d kotak)\n", 
               posisiUlar, posisiBaru - posisiUlar);
        pemain->position = posisiUlar;
        pushPosition(&pemain->history, posisiUlar); // Simpan posisi setelah kena ular
    }
    // Cek tangga
    else {
        int posisiTangga = cekTangga(posisiBaru, tangga, jumlahTangga);
        if (posisiTangga != -1) {
            printf("🪜 BERUNTUNG! Naik tangga di posisi %d!\n", posisiBaru);
            printf("Naik ke posisi: %d (naik %d kotak)\n", 
                   posisiTangga, posisiTangga - posisiBaru);
            pemain->position = posisiTangga;
            pushPosition(&pemain->history, posisiTangga); // Simpan posisi setelah naik tangga
            
            // Cek lagi apakah menang setelah naik tangga
            if (posisiTangga == 100) {
                printf("🎉 SELAMAT! %s MENANG DENGAN TANGGA! 🎉\n", pemain->name);
                printf("==============================\n");
                return 1; // Menang
            }
        }
    }
    
    printf("Posisi akhir: %d\n", pemain->position);
    printf("==============================\n");
    return 0; // Belum menang
}

// Fungsi untuk fitur undo (kembalikan ke posisi sebelumnya)
int undoPemain(Player* pemain) {
    if (isStackEmpty(&pemain->history)) {
        printf("Tidak ada riwayat untuk %s!\n", pemain->name);
        return 0;
    }
    
    int posisiSebelumnya = popPosition(&pemain->history);
    if (posisiSebelumnya != -1) {
        int posisiLama = pemain->position;
        pemain->position = posisiSebelumnya;
        printf("%s dikembalikan dari posisi %d ke posisi %d\n", 
               pemain->name, posisiLama, posisiSebelumnya);
        return 1;
    }
    return 0;
}

// === FUNGSI GAMEPLAY ===

// Fungsi untuk menambah pemain (maksimum 4)
int tambahPemain(PlayerQueue* queue, const char* nama) {
    // Cek apakah sudah mencapai batas maksimum
    if (queue->count >= MAX_PLAYERS) {
        printf("Error: Maksimum %d pemain sudah tercapai!\n", MAX_PLAYERS);
        return 0;
    }
    
    // Cek apakah nama kosong
    if (nama == NULL || strlen(nama) == 0) {
        printf("Error: Nama pemain tidak boleh kosong!\n");
        return 0;
    }
    
    // Cek apakah nama sudah ada
    PlayerNode* current = queue->front;
    while (current != NULL) {
        if (strcmp(current->player.name, nama) == 0) {
            printf("Error: Pemain dengan nama '%s' sudah ada!\n", nama);
            return 0;
        }
        current = current->next;
    }
    
    // Buat pemain baru
    Player pemainBaru = createPlayer(nama);
    
    // Tambahkan ke queue
    if (enqueue(queue, pemainBaru)) {
        printf("Pemain '%s' berhasil ditambahkan! (Posisi: %d)\n", nama, pemainBaru.position);
        return 1;
    } else {
        printf("Error: Gagal menambahkan pemain '%s'!\n", nama);
        return 0;
    }
}

// Fungsi untuk menghapus pemain berdasarkan nama
int hapusPemain(PlayerQueue* queue, const char* nama) {
    if (queue->front == NULL) {
        printf("Error: Tidak ada pemain dalam permainan!\n");
        return 0;
    }
    
    if (nama == NULL || strlen(nama) == 0) {
        printf("Error: Nama pemain tidak boleh kosong!\n");
        return 0;
    }
    
    PlayerNode* current = queue->front;
    PlayerNode* previous = NULL;
    
    // Cari pemain yang akan dihapus
    while (current != NULL) {
        if (strcmp(current->player.name, nama) == 0) {
            // Pemain ditemukan, hapus dari queue
            if (previous == NULL) {
                // Hapus node pertama
                queue->front = current->next;
                if (queue->front == NULL) {
                    queue->rear = NULL; // Queue menjadi kosong
                }
            } else {
                // Hapus node di tengah atau akhir
                previous->next = current->next;
                if (current == queue->rear) {
                    queue->rear = previous; // Update rear jika menghapus node terakhir
                }
            }
            
            printf("Pemain '%s' berhasil dihapus dari permainan!\n", nama);
            free(current);
            queue->count--;
            return 1;
        }
        previous = current;
        current = current->next;
    }
    
    printf("Error: Pemain dengan nama '%s' tidak ditemukan!\n", nama);
    return 0;
}

// Fungsi untuk mencari pemain dan menampilkan posisinya
int cariPemain(PlayerQueue* queue, const char* nama) {
    if (queue->front == NULL) {
        printf("Tidak ada pemain dalam permainan!\n");
        return 0;
    }
    
    if (nama == NULL || strlen(nama) == 0) {
        printf("Error: Nama pemain tidak boleh kosong!\n");
        return 0;
    }
    
    PlayerNode* current = queue->front;
    int urutan = 1;
    
    while (current != NULL) {
        if (strcmp(current->player.name, nama) == 0) {
            printf("=== INFORMASI PEMAIN ===\n");
            printf("Nama: %s\n", current->player.name);
            printf("Posisi saat ini: %d\n", current->player.position);
            printf("Urutan giliran: %d\n", urutan);
            
            // Tampilkan riwayat posisi jika ada
            if (!isStackEmpty(&current->player.history)) {
                printf("Riwayat posisi terakhir: ");
                // Tampilkan beberapa posisi terakhir (maksimal 5)
                int count = 0;
                int tempTop = current->player.history.top;
                while (tempTop >= 0 && count < 5) {
                    printf("%d ", current->player.history.positions[tempTop]);
                    tempTop--;
                    count++;
                }
                printf("\n");
            }
            printf("========================\n");
            return 1;
        }
        current = current->next;
        urutan++;
    }
    
    printf("Pemain dengan nama '%s' tidak ditemukan!\n", nama);
    return 0;
}

// Fungsi untuk menampilkan semua pemain yang sedang bermain
void tampilkanSemuaPemain(PlayerQueue* queue) {
    if (queue->front == NULL) {
        printf("Belum ada pemain dalam permainan!\n");
        return;
    }
    
    printf("\n=== DAFTAR PEMAIN YANG SEDANG BERMAIN ===\n");
    printf("Jumlah pemain: %d/%d\n", queue->count, MAX_PLAYERS);
    printf("==========================================\n");
    
    PlayerNode* current = queue->front;
    int urutan = 1;
    
    while (current != NULL) {
        printf("%d. Nama: %-15s | Posisi: %3d", 
               urutan, current->player.name, current->player.position);
        
        // Tampilkan status khusus berdasarkan posisi
        if (current->player.position == 1) {
            printf(" (Start)");
        } else if (current->player.position == 100) {
            printf(" (MENANG!)");
        } else if (current->player.position >= 90) {
            printf(" (Hampir menang!)");
        }
        
        printf("\n");
        current = current->next;
        urutan++;
    }
    
    printf("==========================================\n\n");
}

// Fungsi untuk mendapatkan pemain giliran berikutnya (tanpa menghapus dari queue)
Player* lihatGiliranBerikutnya(PlayerQueue* queue) {
    if (queue->front == NULL) {
        return NULL;
    }
    return &(queue->front->player);
}

// Fungsi untuk merotasi giliran (pindahkan pemain pertama ke akhir)
void rotasiGiliran(PlayerQueue* queue) {
    if (queue->front == NULL || queue->front->next == NULL) {
        return; // Tidak ada atau hanya satu pemain
    }
    
    // Ambil pemain pertama
    PlayerNode* firstPlayer = queue->front;
    queue->front = queue->front->next;
    
    // Pindahkan ke akhir
    queue->rear->next = firstPlayer;
    queue->rear = firstPlayer;
    firstPlayer->next = NULL;
}

// Fungsi tambahan untuk undo
void undoLastMove(Player* pemain) {
    if (!undoPemain(pemain)) {
        printf("❌ Tidak bisa undo! Riwayat kosong untuk %s.\n", pemain->name);
    } else {
        printf("✅ Undo berhasil untuk %s.\n", pemain->name);
    }
}

// Fungsi untuk menampilkan papan permainan
void tampilkanPapan(PlayerQueue* queue, SnakeOrLadder ular[], int jumlahUlar, SnakeOrLadder tangga[], int jumlahTangga) {
    printf("\n=== PAPAN PERMAINAN ===\n");
    for (int row = 9; row >= 0; row--) {
        for (int col = 0; col < 10; col++) {
            int pos = row * 10 + (row % 2 == 0 ? col + 1 : 10 - col); // Zigzag pattern

            // Cek pemain
            PlayerNode* curr = queue->front;
            int found = 0;
            while (curr != NULL) {
                if (curr->player.position == pos) {
                    printf("[P%c]", curr->player.name[0]);
                    found = 1;
                    break;
                }
                curr = curr->next;
            }
            if (found) continue;

            // Cek ular
            int isUlar = 0;
            for (int i = 0; i < jumlahUlar; i++) {
                if (ular[i].start == pos) {
                    printf("[↓ ]");
                    isUlar = 1;
                    break;
                }
            }
            if (isUlar) continue;

            // Cek tangga
            int isTangga = 0;
            for (int i = 0; i < jumlahTangga; i++) {
                if (tangga[i].start == pos) {
                    printf("[↑ ]");
                    isTangga = 1;
                    break;
                }
            }
            if (isTangga) continue;

            // Kosong
            printf("[%02d]", pos);
        }
        printf("\n");
    }
    printf("========================\n\n");
}

int menuManajemenPemain(PlayerQueue* queue) {
    int pilihan;
    char nama[MAX_NAME_LENGTH];

    do {
        printf("\n=== MENU MANAJEMEN PEMAIN ===\n");
        printf("1. Tambah Pemain\n");
        printf("2. Hapus Pemain\n");
        printf("3. Cari Pemain\n");
        printf("4. Tampilkan Semua Pemain\n");
        printf("5. Mulai Permainan\n");
        printf("0. Keluar\n");
        printf("Pilihan Anda: ");
        scanf("%d", &pilihan);
        getchar(); // buang newline

        switch (pilihan) {
            case 1:
                printf("Masukkan nama pemain: ");
                fgets(nama, MAX_NAME_LENGTH, stdin);
                nama[strcspn(nama, "\n")] = '\0'; // hapus newline
                tambahPemain(queue, nama);
                break;
            case 2:
                printf("Masukkan nama pemain yang ingin dihapus: ");
                fgets(nama, MAX_NAME_LENGTH, stdin);
                nama[strcspn(nama, "\n")] = '\0';
                hapusPemain(queue, nama);
                break;
            case 3:
                printf("Masukkan nama pemain yang ingin dicari: ");
                fgets(nama, MAX_NAME_LENGTH, stdin);
                nama[strcspn(nama, "\n")] = '\0';
                cariPemain(queue, nama);
                break;
            case 4:
                tampilkanSemuaPemain(queue);
                break;
            case 5:
                if (getPlayerCount(queue) < 2) {
                    printf("Minimal 2 pemain untuk memulai permainan!\n");
                } else {
                    return 1; // Mulai permainan
                }
                break;
            case 0:
                return 0; // Keluar program
            default:
                printf("Pilihan tidak valid!\n");
        }
    } while (1);
}

void jalankanPermainan(PlayerQueue* queue, SnakeOrLadder ular[], int jumlahUlar, SnakeOrLadder tangga[], int jumlahTangga) {
    int gameSelesai = 0;

    while (!gameSelesai) {
        Player* pemain = lihatGiliranBerikutnya(queue);
        if (pemain == NULL) break;

        printf("\nGiliran: %s (Posisi: %d)\n", pemain->name, pemain->position);
        printf("Tekan ENTER untuk lempar dadu...");
        getchar();

        int dadu = lemparDadu();
        printf("🎲 Dadu menunjukkan angka: %d\n", dadu);

        int menang = pindahkanPemain(pemain, dadu, ular, jumlahUlar, tangga, jumlahTangga);
        tampilkanPapan(queue, ular, jumlahUlar, tangga, jumlahTangga);

        if (menang) {
            gameSelesai = 1;
            break;
        }

        // Opsi undo
        char pilihan;
        printf("Apakah Anda ingin melakukan UNDO? (y/n): ");
        scanf(" %c", &pilihan);
        getchar();
        if (pilihan == 'y' || pilihan == 'Y') {
            undoLastMove(pemain);
            tampilkanPapan(queue, ular, jumlahUlar, tangga, jumlahTangga);
        }

        rotasiGiliran(queue);
    }

    printf("Permainan selesai. Terima kasih telah bermain!\n");
}


int main() {
    srand(time(NULL));

    PlayerQueue gameQueue;
    initQueue(&gameQueue);

    SnakeOrLadder ular[20];
    SnakeOrLadder tangga[20];
    int jumlahUlar, jumlahTangga;

    printf("🎮 SELAMAT DATANG DI GAME ULAR TANGGA 🎲\n");

    inisialisasiUlar(ular, &jumlahUlar);
    inisialisasiTangga(tangga, &jumlahTangga);

    int mulai = menuManajemenPemain(&gameQueue);

    if (mulai) {
        printf("\n=== PERMAINAN DIMULAI! ===\n");
        tampilkanPapan(&gameQueue, ular, jumlahUlar, tangga, jumlahTangga);
        jalankanPermainan(&gameQueue, ular, jumlahUlar, tangga, jumlahTangga);
    } else {
        printf("Keluar dari permainan. Sampai jumpa!\n");
    }

    freeQueue(&gameQueue);
    return 0;
}
