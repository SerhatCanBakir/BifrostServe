# C HTTP Sunucusu

Bu proje, temel bir HTTP sunucusunu C dili ile `Winsock2` kullanarak geliştirmek amacıyla oluşturulmuştur. **Ana hedef**, soket programlama, bellek yönetimi ve çoklu istemci desteği alanlarında kendimi geliştirerek daha derin bir anlayış kazanmaktır.

## 📌 Projenin Amacı ve Kazanımlar
- **Düşük seviyeli ağ programlama**: `Winsock2` kullanarak TCP/IP tabanlı bir HTTP sunucusu inşa etme pratiği.
- **Dinamik bellek yönetimi**: `malloc()` ve `free()` fonksiyonlarıyla bellek ayırma ve sızıntıları önlemek için `appDestroy()` fonksiyonunun eklenmesi.
- **Çoklu istemci desteği**: `WSAPoll()` kullanarak birden fazla istemciyle aynı anda iletişim kurma.
- **Dosya okuma ve sunma**: HTML ve JavaScript dosyalarını sunarak temel bir web sunucusu işlevi.
- **Modüler yapı**: `app.h` ve `app.c` dosyaları sayesinde kod organizasyonunun daha net olması.
- **Dinamik yanıt desteği**: `createResponseDynamic` fonksiyonu sayesinde fonksiyon çıktıları yanıt olarak döndürülebilir.

Bu süreç, **backend geliştirme** ve **düşük seviyeli programlama** alanlarında deneyim kazanmama yardımcı olmuştur.

---

## 🛠 **Kurulum ve Çalıştırma**

### **1. Gerekli Kütüphaneler**
Bu proje `Winsock2` kütüphanesini kullanır. Sadece Windows üzerinde derlenip çalıştırılması tavsiye edilir.

### **2. Derleme**
Projeyi derlemek için `gcc` kullanabilirsiniz:
```sh
gcc main.c app.c -o server.exe -lws2_32
```

### **3. Çalıştırma**
Sunucuyu başlatmak için:
```sh
server.exe
```

Varsayılan olarak, sunucu **127.0.0.1:8080** adresinde dinlemeye başlayacaktır (bkz. `startServer(&myApp, "127.0.0.1", 8080);`).

---

## 📜 **Kod Açıklaması**

### **🔹 1. `main.c`**
Bu dosya, uygulamanın başlangıç noktasıdır. Sunucuyu başlatır, istek/yanıt oluşturur ve `startServer` fonksiyonunu çağırarak dinleme döngüsünü başlatır.

Örnek satırlar:
```c
APP myApp;
appInit(&myApp);

struct request req = *createRequest("GET", "/", " ", " ");
char *fileContent = readFile("./site.html");
struct response res = *createResponse(200, " ", fileContent);

appendrequest(&myApp, req, res);
startServer(&myApp, "127.0.0.1", 8080);

appDestroy(&myApp);  // Sunucu kapatıldığında belleği temizle
```

### **🔹 2. `app.h` (Başlık Dosyası)**
**Önemli Yapılar ve Fonksiyonlar:**

- **`struct request`** ve **`struct response`**: HTTP istek ve yanıt bilgilerini tutar.
- **`APP`** yapısı: Sunucunun sahip olduğu `request` ve `response` dizilerini ve bunların sayacı (`reqSize`, `resSize`) tutar.
- **`int appInit(APP *app)`**: Uygulamayı başlatır ve ilgili bellekleri tahsis eder.
- **`int appendrequest(APP *app, struct request req, struct response res)`**: Yeni istek ve yanıtı sunucuya ekler.
- **`int startServer(APP *app, char *ipAddr, int PORT)`**: Sunucuyu başlatır ve gelen istekleri dinler.
- **`void appDestroy(APP *app)`**: Sunucu kapandığında tüm tahsis edilmiş belleği temizler.
- **`struct request *createRequest(...)`** ve **`struct response *createResponse(...)`**: Dinamik bellek ile yeni bir istek ya da yanıt oluşturur.
- **`struct response* createResponseDynamic(int status, char *contentType, char *body, callBackFunc callbackfunc, int callbackCount, void** args);`**: Dinamik içerik oluşturulmasını sağlar.
- **`char* readFile(const char *filename)`**: Dosyayı okuyup bellek alanına aktarır.

### **🔹 3. `response` Yapısında Güncelleme**
Yeni `struct response` yapısı:
```c
struct response {
    int isStatic;
    int status;
    char *contentType;
    unsigned int contentLenght;
    char *body;
    callBackFunc callbackfunc;
    int callbackCount;
    void** args;
};
```
Bu yapı sayesinde artık response içine fonksiyon atayıp, her çağrıldığında callback şeklinde çalışmasını sağlayabiliyoruz.

---

## 🧪 **Test Etme**
1. Derledikten sonra `server.exe` çalıştırın.
2. Tarayıcı veya `curl` aracı ile `http://127.0.0.1:8080/` adresine istek atın.
3. Eğer `site.html` mevcut ve kod düzgün çalışıyorsa, HTTP 200 yanıtı ve HTML içeriği döner.

Örnek:
```sh
curl http://127.0.0.1:8080/
```

---

## 🚀 **Geliştirme Planı**
Bu proje, ağ programlama ve bellek yönetimi konularında kendimi geliştirmek için oluşturuldu. İleride eklemeyi düşündüğüm bazı özellikler:

- [ ] POST isteklerini işleme
- [ ] HTTP 1.1 Keep-Alive
- [ ] Gelişmiş header yönetimi
- [ ] Farklı MIME türlerini otomatik tespit etme

---

## 📜 **Lisans**
Bu proje açık kaynak olarak sunulmuştur. **Kendi projelerinde kullanabilir**, dilediğin şekilde geliştirebilirsin.

---

## 💬 **Geri Bildirim**
Projeyle veya belgelerle ilgili her türlü geri bildiriminizi bekliyorum! Özellikle bellek yönetimi, kod düzeni veya ek özellikler konusunda önerileriniz olursa katkıda bulunabilirsiniz.

