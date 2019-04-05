# SoalShift_modul3_E08

#### Bagas Yanuar Sudrajad - 05111740000074 
#### Octavianus Giovanni Yaunatan - 05111740000113

## SOAL 1

Buatlah program C yang bisa menghitung faktorial secara parallel lalu menampilkan hasilnya secara berurutan

Contoh:

​	./faktorial 5 3 4
​	<br>3! = 6
​	<br>4! = 24
​	<br>5! = 120

### Langkah-langkah

1. Baca masukan angka, simpan pada sebuah list untuk masalah yang akan diselesaikan.
2. Urutkan list masalah.
3. Kerjakan satu masalah menggunakan satu thread.
4. Cetak hasil perhitungan, sesuai antrian.

### Implementasi

- Deklarasikan variabel-variabel yang akan digunakan

  ```c
  pthread_t tid[100];
  int ans[100];
  int masalah[100];
  int pekerja;
  int antrian = 1;
  ```

- Catat jumlah masalah (=jumlah pekerja/thread yang dibutuhkan). Convert inputan yang disimpan dalam bentuk *string* ke *integer*. Urutkan `masalah` .

  ```c
  pekerja=argc;
  for(int i = 1; i < argc; i++){
      char *p;
      int conv = strtol(argv[i], &p, 10);
      masalah[i]=conv;
  }
  qsort(masalah,argc,sizeof(int), cmpfunc); 
  ```

- Buat thread, jalankan fungsi factorial. Join thread-thread agar proses utama menunggu semua thread selesai.

  ```c
  for(int i=1; i < argc; i++){
  	    pthread_create(&(tid[i]),NULL,&factorial,NULL);
  	}
  	for(int i=1; i<argc; i++){
          pthread_join(tid[i],NULL);
  	}
  ```

- Comparator untuk pengurutan yang digunakan.

  ```c
  int cmpfunc(const void * a, const void * b){
  	return (*(int*)a - *(int*)b);
  }
  ```

- Fungsi factorial yang dijalankan tiap thread

  ```c
  void* factorial(void *arg)
  {
  	pthread_t id=pthread_self();
  	int idx; // untuk no urut.
  	for(int i = 1; i < pekerja ; i++){
  		if(pthread_equal(id,tid[i])){ // thread mengerjakan masalah sesuai index masalah dan index thread
  			ans[i]=1;	
              idx=i;
  			for(int j = 1; j <= masalah[i]; j++){
  				ans[i]*=j;
  			}
              while(antrian != i); // tunggu antrian
              printf("%d! = %d\n", masalah[i],ans[i]); 
              antrian++;
  		}
      }
  	return NULL;
  }
  ```

  

[Code untuk soal 1](https://github.com/Axerion1210/SoalShift_modul3_E08/blob/master/Jawaban/soal1/soal1.c)

## SOAL 2

Pada suatu hari ada orang yang ingin berjualan 1 jenis barang secara private, dia memintamu membuat program C dengan spesifikasi sebagai berikut:

1. 1. Terdapat 2 server: server penjual dan server pembeli
   2. 1 server hanya bisa terkoneksi dengan 1 client
   3. Server penjual dan server pembeli memiliki stok barang yang selalu sama
   4. Client yang terkoneksi ke server penjual hanya bisa menambah stok
   5. - Cara menambah stok: client yang terkoneksi ke server penjual mengirim string “tambah” ke server lalu stok bertambah 1
   6. Client yang terkoneksi ke server pembeli hanya bisa mengurangi stok
   7. - Cara mengurangi stok: client yang terkoneksi ke server pembeli mengirim string “beli” ke server lalu stok berkurang 1
   8. Server pembeli akan mengirimkan info ke client yang terhubung dengannya apakah transaksi berhasil atau tidak berdasarkan ketersediaan stok
   9. - Jika stok habis maka client yang terkoneksi ke server pembeli akan mencetak “transaksi gagal”
      - Jika stok masih ada maka client yang terkoneksi ke server pembeli akan mencetak “transaksi berhasil”
   10. Server penjual akan mencetak stok saat ini setiap 5 detik sekali
   11. **Menggunakan thread, socket, shared memory**

### Langkah-langkah

Terdapat program untuk Server Penjual, Client Penjual, Server Pembeli, Client Pembeli dengan hubungan sebagai berikut.

1. Server Penjual terkoneksi dengan Client Penjual, begitu juga yang pembeli
2. Server Penjual dan Pembeli terkoneksi menggunakan shared memory (berbagi informasi stock)

### Implementasi

#### Server Penjual 

- Variabel-variabel yang dibutuhkan

  ```c
  key_t key = 1330;
  //int *value;
  int *stock;
  pthread_t tid[3];
  
  
  int server_fd, new_socket, valread;
      struct sockaddr_in address;
      int opt = 1;
      int addrlen = sizeof(address);
      char buffer[1024] = {0};
      char *hello = "Hello from server";
      char *tambah = "tambah";
      char *keluar = "keluar";
      char banyakStock[100];
  ```

- Server Penjual. Atur koneksi dengan client, lalu jalankan program seperti berikut

  ```c
  // atur shared memory
  int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
  stock = shmat(shmid, NULL, 0); 
  *stock=0;
  
  pthread_create(&(tid[0]),NULL,&terima,NULL); //buat thread untuk read
  pthread_create(&(tid[1]),NULL,&kirim,NULL);  //buat thread untuk send
  
  pthread_join(tid[0],NULL); //tunggu thread read
  //pthread_join(tid[1],NULL);
  shmdt(stock); //lepas koneksi shared memory
  shmctl(shmid, IPC_RMID, NULL); // hapus memory
  ```

- Fungsi read yang dijalankan thread

  ```c
  void* terima(void *arg)
  {
  	unsigned long i=0;
  	pthread_t id=pthread_self();
  	if(pthread_equal(id,tid[0]))
  	{   
          int flag = 1; //flag untuk terminate
          while(flag){
              valread = read( new_socket , buffer, 1024); //baca dari client
              if(strcmp(buffer,tambah)==0){  // jika client request tambah
                  *stock=*stock+1; // tambah stock
                  //printf("Masuk bos\n, stock: %d", *stock);
              }
              else if(strcmp(buffer,keluar)==0){ // jika client selesai
                  flag=0; // selesai
              }
          }
  	}
  	return NULL;
  }
  ```

- Fungsi send yang dijalankan thread

  ```c
  void* kirim(void *arg)
  {
  	unsigned long i=0;
  	pthread_t id=pthread_self();
  	if(pthread_equal(id,tid[1])) 
  	{   
          int flag = 1; //flag untuk terminate
          while(flag){
              sleep(5); // setiap lima detik
              sprintf(banyakStock,"%d",*stock); //ambil nilai stock
              send(new_socket , banyakStock , strlen(banyakStock) , 0 ); // kirim
          }
  	}
  	return NULL;
  }
  
  ```

#### Client Penjual

- Deklarasi variabel yang dibutuhkan

  ```c
  #define PORT 8080
  
  pthread_t tid[3];
  int stock = 0;
   struct sockaddr_in address;
      int sock = 0, valread;
      struct sockaddr_in serv_addr;
  	//pesan-pesan
      char *hello = "Hello from client";
      char *tambah = "tambah";
      char *tes = "tes";
      char *keluar = "keluar";
      char buffer[1024] = {0};
  ```

- Atur koneksi dengan server, lalu jalankan program seperti berikut

  ```c
  pthread_create(&(tid[0]),NULL,&kirim,NULL); //buat thread untuk send
  pthread_create(&(tid[1]),NULL,&terima,NULL);//buat thread untuk receive
  pthread_join(tid[0],NULL); // tunggu thread 0 untuk selesai
  ```

- Fungsi send yang dijalankan thread

  ```c
  void* kirim(void *arg)
  {
  	unsigned long i=0;
  	pthread_t id=pthread_self();
  	if(pthread_equal(id,tid[0]))
  	{
  		int flag = 1; //flag untuk terminasi
          int choice; //pilihan aksi
          while (flag){
              system("clear");//clear terminal
              printf("\n1. Tambah Stock.\n-1. Quit\nChoice: ");//print pilihan
              scanf("%d", &choice);//baca pilihan user
              if(choice==1){//jika tambah
                  send(sock, tambah, strlen(tambah) , 0 );//kirim pesan untuk tambah
              }else if(choice == -1){ // jika keluar
                  flag=0; // terminate while
                  send(sock , keluar , strlen(keluar) , 0 );//kirim pesan untuk keluar
              }
              fflush(stdout);
         }
  	}
  	return NULL;
  }
  ```

- Fungsi receive yang dijalankan thread

  ```c
  void* terima(void *arg)
  {
  	unsigned long i=0;
  	pthread_t id=pthread_self();
  	if(pthread_equal(id,tid[1])) //thread untuk menjalankan counter
  	{
  		int flag = 1;
          while (flag){
              valread = read( sock , buffer, 1024); //baca pesan dari server
              if(valread!=0){
                  system("clear");//clear
                  printf("Banyak stock: %s\n",buffer); // cetak banyak stock
                  printf("1. Tambah Stock.\n-1. Quit\nChoice: "); //cetak pilihan aksi
                  fflush(stdout);
              }
              memset(buffer, 0, 1024*sizeof(buffer[0])); //reset buffer
          }
  	}
  	return NULL;
  }
  ```

#### Server Pembeli

- Deklarasi variabel yang dibutuhkan

  ```c
  #define PORT 8081
  key_t key = 1330;
  int *stock;
  pthread_t tid[3];
  int server_fd, new_socket, valread;
      struct sockaddr_in address;
      int opt = 1;
      int addrlen = sizeof(address);
      char buffer[1024] = {0};
      char *sukses = "Pembelian sukses Gan!";
      char *kurang = "kurang";
      char *keluar = "keluar";
      char *gagal = "Stock abis gan :(";
      char banyakStock[100];
  ```

- Atur koneksi dengan client lalu jalankan program seperti berikut

  ```c
  int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666); // atur shared memory dengan server penjual
  stock = shmat(shmid, NULL, 0);
  *stock=0;
  pthread_create(&(tid[0]),NULL,&terima,NULL);//buat thread untuk terima dan kirim
  pthread_join(tid[0],NULL);
  //pthread_join(tid[1],NULL);
  shmdt(stock); // lepas shared memory
  shmctl(shmid, IPC_RMID, NULL); // hapus memory
  ```

- Fungsi untuk receive dan send yang dijalankan oleh thread

  ```c
  void* terima(void *arg)
  {
  	unsigned long i=0;
  	pthread_t id=pthread_self();
  	if(pthread_equal(id,tid[0]))
  	{   
          int flag = 1;
          while(flag){
              valread = read( new_socket , buffer, 1024); //terima pesan dari client
              if(strcmp(buffer,kurang)==0){ // jika client req beli
                  if(*stock>0){ // jika masih ada stock
                      *stock =*stock - 1; //kurangi stock
                      send(new_socket , sukses , strlen(sukses) , 0 ); // kirim pesan sukses
                  }else{
                      send(new_socket , gagal , strlen(gagal) , 0 ); // kirim pesan gagal 
                  }
              }
              else if(strcmp(buffer,keluar)==0){ // jika keluar
                  flag=0; // flag untuk keluar
              }
              memset(buffer, 0, 1024*sizeof(buffer[0])); 
          }
  	}
  	return NULL;
  }
  ```

#### Client Pembeli

- Deklarasi variabel yang dibutuhkan

  ```c
  #define PORT 8081
  
  pthread_t tid[3];
  int stock = 0;
  struct sockaddr_in address;
      int sock = 0, valread;
      struct sockaddr_in serv_addr;
      char *hello = "Hello from client";
      char *kurang = "kurang";
      char *tes = "tes";
      char *keluar = "keluar";
      char buffer[1024] = {0};
  ```

- Atur koneksi dengan server lalu jalankan program seperti berikut

  ```c
  pthread_create(&(tid[0]),NULL,&kirim,NULL); //buat thread untuk terima dan kirim
  pthread_join(tid[0],NULL); // tunggu thread selesai
  ```

- Fungsi yang dijalankan oleh thread

  ```c
  void* kirim(void *arg)
  {
  	unsigned long i=0;
  	pthread_t id=pthread_self();
  	if(pthread_equal(id,tid[0]))
  	{
  		int flag = 1; //flag untuk terminate
          int choice; // pilihan aksi user
          while (flag){
              system("clear");
              printf("%s\n",buffer);//print pesan dari server
              printf("\n1. Beli gan!.\n-1. Quit\nChoice: ");//print pilihan aksi
              scanf("%d", &choice); // ambil pilihan aksi
              if(choice==1){ // jika beli
                  send(sock, kurang, strlen(kurang) , 0 ); // kirim pesan
                  valread = read( sock , buffer, 1024); // terima pesan
              }else if(choice == -1){ // jika keluar
                  flag=0; // keluar
                  send(sock , keluar , strlen(keluar) , 0 ); // kirim keluar
              }
              fflush(stdout);
              memset(buffer, 0, 1024*sizeof(buffer[0])); 
         }
  	}
  	return NULL;
  }
  ```

## SOAL 3

Agmal dan Iraj merupakan 2 sahabat yang sedang kuliah dan hidup satu kostan, sayangnya mereka mempunyai gaya hidup yang berkebalikan, dimana Iraj merupakan laki-laki yang sangat sehat,rajin berolahraga dan bangun tidak pernah kesiangan sedangkan Agmal hampir menghabiskan setengah umur hidupnya hanya untuk tidur dan ‘ngoding’. Dikarenakan mereka sahabat yang baik, Agmal dan iraj sama-sama ingin membuat satu sama lain mengikuti gaya hidup mereka dengan cara membuat Iraj sering tidur seperti Agmal, atau membuat Agmal selalu bangun pagi seperti Iraj. Buatlah suatu program C untuk menggambarkan kehidupan mereka dengan spesifikasi sebagai berikut:

1. Terdapat 2 karakter Agmal dan Iraj
2. Kedua karakter memiliki status yang unik
    - Agmal mempunyai WakeUp_Status, di awal program memiliki status 0
    - Iraj memiliki Spirit_Status, di awal program memiliki status 100
    - Terdapat 3 Fitur utama
        - All Status, yaitu menampilkan status kedua sahabat
          <br>Ex:
          <br>Agmal WakeUp_Status = 75 
          <br>Iraj Spirit_Status = 30
        - “Agmal Ayo Bangun” menambah WakeUp_Status Agmal sebesar 15 point
        - “Iraj Ayo Tidur” mengurangi Spirit_Status Iraj sebanyak 20 point
    - Terdapat Kasus yang unik dimana:
        - Jika Fitur “Agmal Ayo Bangun” dijalankan sebanyak 3 kali, maka Fitur “Iraj Ayo Tidur” Tidak bisa dijalankan selama 10 detik (Dengan mengirim pesan ke sistem “Fitur Iraj Ayo Tidur disabled 10 s”)
        - Jika Fitur  “Iraj Ayo Tidur” dijalankan sebanyak 3 kali, maka Fitur “Agmal Ayo Bangun” Tidak bisa dijalankan selama 10 detik (Dengan mengirim pesan ke sistem “Agmal Ayo Bangun disabled 10 s”)
    - Program akan berhenti jika Salah Satu :
        - WakeUp_Status Agmal >= 100 (Tampilkan Pesan “Agmal Terbangun,mereka bangun pagi dan berolahraga”)
        - Spirit_Status Iraj <= 0 (Tampilkan Pesan “Iraj ikut tidur, dan bangun kesiangan bersama Agmal”)
    - Syarat Menggunakan Lebih dari 1 Thread

### Langkah-Langkah

1. Buat tiga thread, yaitu status, bangun, tidur.
2. Thread status untuk menjalankan fitur All Status yang berisi status dari variabel WakeUp_Status dan Spirit_Status.
3. Thread bangun untuk menjalankan fitur Agmal Ayo bangun, akan disleep jika Iraj Ayo Tidur dijalankan sebanyak 3 kali.
4. Thread tidur untuk menjalankan fitur Iraj Ayo Tidur, akan disleep jika Agmal Ayo Bangun dijalankan sebanyak 3 kali.
5. Membuat input untuk menjalankan salah satu fitur.

### Implementasi



## SOAL 4

Buatlah sebuah program C dimana dapat menyimpan list proses yang sedang berjalan (ps -aux) maksimal 10 list proses. Dimana awalnya list proses disimpan dalam di 2 file ekstensi .txt yaitu  SimpanProses1.txt di direktori /home/Document/FolderProses1 dan SimpanProses2.txt di direktori /home/Document/FolderProses2 , setelah itu masing2 file di  kompres zip dengan format nama file KompresProses1.zip dan KompresProses2.zip dan file SimpanProses1.txt dan SimpanProses2.txt akan otomatis terhapus, setelah itu program akan menunggu selama 15 detik lalu program akan mengekstrak kembali file KompresProses1.zip dan KompresProses2.zip 

Dengan Syarat :  

- Setiap list proses yang di simpan dalam masing-masing file .txt harus berjalan bersama-sama
- Ketika mengkompres masing-masing file .txt harus berjalan bersama-sama
- Ketika Mengekstrak file .zip juga harus secara bersama-sama
- Ketika Telah Selesai melakukan kompress file .zip masing-masing file, maka program akan memberi pesan “Menunggu 15 detik untuk mengekstrak kembali”
- Wajib Menggunakan Multithreading
- Boleh menggunakan system

### Langkah-langkah

1. Buat dua thread untuk mengambil data dan memasukkannya pada file tujuan.
2. Setelah nomor 1 selesai, buat dua thread untuk melakukan kompresi.
3. Setelah nomor 2 selesai, tunggu 15 detik. Lalu buat 2 trhead untuk melakukan ekstraksi.

### Implementasi

- fungsi main. 

  ```c
  pthread_t tid[10]; //inisialisasi array untuk menampung thread dalam kasus ini ada 2 thread
  int flag=1;
  
  int main(void)
  {
  	int i=0;
  	int err;
  	while(i<2) // loop untuk 2 thread
  	{
  		err=pthread_create(&(tid[i]),NULL,&buatFile,NULL); //thread untuk buat file
  		i++;
  	}
      //tunggu 2 thread selesai
  	pthread_join(tid[0],NULL);
  	pthread_join(tid[1],NULL);
  	while(i<4) // loop untuk 2 thread
  	{
  		err=pthread_create(&(tid[i]),NULL,&kompresFile,NULL); //thread untuk kompres file
  		i++;
  	}
      // tunggu 2 thread selesai
      pthread_join(tid[2],NULL);
  	pthread_join(tid[3],NULL);
  	//tunggu 15 detik
      printf("\n\nMenunggu 15 detik untuk mengekstrak kembali");
      int timer=0;
      while (timer < 15){
          printf(". ");
          fflush(stdout);
          ++timer;
          sleep(1);
      }
      printf("\n");
      
      while(i<6) // loop untuk 2 thread
  	{
  		err=pthread_create(&(tid[i]),NULL,&ekstrakFile,NULL); //thread untuk ekstrak file
  		i++;
  	}
      //tunggu 2 thread selesai
  	pthread_join(tid[4],NULL);
  	pthread_join(tid[5],NULL);
      //exit(0);
  	return 0;
  }
  ```

- Fungsi untuk membuat file

  ```c
  void* buatFile(void *arg)
  {
  	unsigned long i=0;
  	pthread_t id=pthread_self();
  	int iter;
  
  	if(pthread_equal(id,tid[0])) // buat foler, ps -aux, ambil 10, masukkan file
  	{
  		system("mkdir -p /home/siung2/Documents/FolderProses1 && ps -aux | head -10 > /home/siung2/Documents/FolderProses1/SimpanProses1.txt");
  	}
  	else if(pthread_equal(id,tid[1]))// buat foler, ps -aux, ambil 10, masukkan file
  	{
  		system("mkdir -p /home/siung2/Documents/FolderProses2 && ps -aux | head -10 > /home/siung2/Documents/FolderProses2/SimpanProses2.txt");		
  	}
  	return NULL;
  }
  ```

- Fungsi untuk zip file

  ```c
  void* kompresFile(void *arg)
  {
  	unsigned long i=0;
  	pthread_t id=pthread_self();
  	int iter;
  
  	if(pthread_equal(id,tid[2])) // kompres file
  	{
  		system("cd /home/siung2/Documents/FolderProses1 && zip -m KompresProses1.zip SimpanProses1.txt");
  	}
  	else if(pthread_equal(id,tid[3])) //  kompres file
  	{	
  		system("cd /home/siung2/Documents/FolderProses2 && zip -m KompresProses2.zip SimpanProses2.txt");
  	}
  	return NULL;
  }
  ```

- Fungsi untuk ekstrak

  ```c
  void* ekstrakFile(void *arg)
  {
  	unsigned long i=0;
  	pthread_t id=pthread_self();
  	int iter;
  	if(pthread_equal(id,tid[4])) // ekstrak file
  	{
  		system("unzip -o /home/siung2/Documents/FolderProses1/KompresProses1.zip -d /home/siung2/Documents/FolderProses1");
  	}
  	else if(pthread_equal(id,tid[5])) // ekstrak file
  	{	
  		system("unzip -o /home/siung2/Documents/FolderProses2/KompresProses2.zip -d /home/siung2/Documents/FolderProses2");
  	}
  	return NULL;
  }
  ```

### SOAL 5

Angga, adik Jiwang akan berulang tahun yang ke sembilan pada tanggal 6 April besok. Karena lupa menabung, Jiwang tidak mempunyai uang sepeserpun untuk membelikan Angga kado. Kamu sebagai sahabat Jiwang ingin membantu Jiwang membahagiakan adiknya sehingga kamu menawarkan bantuan membuatkan permainan komputer sederhana menggunakan program C. Jiwang sangat menyukai idemu tersebut. Berikut permainan yang Jiwang minta. 
1. Pemain memelihara seekor monster lucu dalam permainan. Pemain dapat  memberi nama pada monsternya.
2. Monster pemain memiliki hunger status yang berawal dengan nilai 200 (maksimalnya) dan nanti akan berkurang 5 tiap 10 detik.Ketika hunger status mencapai angka nol, pemain akan kalah. Hunger status dapat bertambah 15 apabila pemain memberi makan kepada monster, tetapi banyak makanan terbatas dan harus beli di Market.
3. Monster pemain memiliki hygiene status yang berawal dari 100 dan nanti berkurang 10 tiap 30 detik. Ketika hygiene status mencapai angka nol, pemain akan kalah. Hygiene status' dapat bertambah 30 hanya dengan memandikan monster. Pemain dapat memandikannya setiap 20 detik(cooldownnya 20 detik).
4. Monster pemain memiliki health status yang berawal dengan nilai 300. Variabel ini bertambah (regenerasi)daa 5 setiap 10 detik ketika monster dalam keadaan standby.
5. Monster pemain dapat memasuki keadaan battle. Dalam keadaan ini, food status(fitur b), hygiene status'(fitur c), dan ‘regenerasi’(fitur d) tidak akan berjalan. Health status dari monster dimulai dari darah saat monster pemain memasuki battle. Monster pemain akan bertarung dengan monster NPC yang memiliki darah 100. Baik monster pemain maupun NPC memiliki serangan sebesar 20. Monster pemain dengan monster musuh akan menyerang secara bergantian. 
6. Fitur shop, pemain dapat membeli makanan sepuas-puasnya selama stok di toko masih tersedia.
    - Pembeli (terintegrasi dengan game)
        - Dapat mengecek stok makanan yang ada di toko.
        - Jika stok ada, pembeli dapat membeli makanan.
    - Penjual (terpisah)
        - Bisa mengecek stok makanan yang ada di toko
        - Penjual dapat menambah stok makanan.

Spesifikasi program:

1. Program mampu mendeteksi input berupa key press. (Program bisa berjalan tanpa perlu menekan tombol enter)
2. Program terdiri dari 3 scene yaitu standby, battle, dan shop.
3. Pada saat berada di standby scene, program selalu menampilkan health status, hunger status, hygiene status, stok makanan tersisa, dan juga status kamar mandi (“Bath is ready” jika bisa digunakan, “Bath will be ready in [bath cooldown]s” jika sedang cooldown). Selain itu program selalu menampilkan 5 menu, yaitu memberi makan, mandi, battle, shop, dan exit. Contoh :

```
Standby Mode
Health : [health status]
Hunger : [hunger status]
Hygiene : [hygiene status]
Food left : [your food stock]
Bath will be ready in [cooldown]s
Choices
1. Eat
2. Bath
3. Battle
4. Shop
5. Exit
```

4. Pada saat berada di battle scene, program selalu menampilkan health status milik pemain dan monster NPC. Selain itu, program selalu menampilkan 2 menu yaitu serang atau lari. Contoh :

```
Battle Mode
Monster’s Health : [health status]
Enemy’s Health : [enemy health status]
Choices
1. Attack
2. Run
```

5. Pada saat berada di shop scene versi pembeli, program selalu menampilkan food stock toko dan milik pemain. Selain itu, program selalu menampilkan 2 menu yaitu beli dan kembali ke standby scene. Contoh :

```
Shop Mode
Shop food stock : [shop food stock]
Your food stock : [your food stock]
Choices
1. Buy
2. Back
```

6. Pada program penjual, program selalu menampilkan food stock toko. Selain itu, program juga menampilkan 2 menu yaitu restock dan exit. Contoh :

```
Shop
Food stock : [shop food stock]
Choices
1. Restock
2. Exit
```

7. Pastikan terminal hanya mendisplay status detik ini sesuai scene terkait (hint: menggunakan system(“clear”))

### Langkah-Langkah

1. 

### Implementasi
