# ft_irc

## Giriş

ft_irc, internet üzerinde gerçek zamanlı metin tabanlı iletişim sağlayan IRC (Internet Relay Chat) protokolü üzerine inşa edilmiş bir uygulamadır. Bu proje, kullanıcıların doğrudan mesaj alışverişi yapmasını ve grup kanallarına katılmasını sağlar. IRC istemcileri, IRC sunucusuna bağlanarak kanallara katılabilirler. IRC sunucuları, birbirleriyle bağlanarak bir ağ oluştururlar.

## Soket Programlama Nedir?

Soket programlama, ağ tabanlı uygulamaların geliştirilmesinde kullanılan bir tekniktir. Client-server mantığına dayanır; client, bir hizmete erişmek isteyen bir bilgisayarı, server ise bir hizmeti sağlayan bilgisayarı temsil eder. Soketler, client ve server arasındaki iletişimi sağlar.

İletişim şu adımlarla gerçekleşir:

1. Server soketi oluşturma: Server, belirli bir port üzerinden gelecek clientlara hizmet vereceği için bir soket oluşturur ve bir port numarası belirler.
2. Server soketini bağlama: Server soketi, belirlenen port numarasına bağlanır ve gelecek clientları dinlemeye başlar.
3. Client soketi oluşturma: Client, serverla iletişim kurmak için bir soket oluşturur.
4. Client soketini servera bağlama: Client soketi, serverın IP adresi ve port numarasıyla bağlantı kurar.
5. Client ve server arasında veri iletişimi: Client ve server, soketleri üzerinden veri alışverişi yaparlar. Client, servera istek gönderir ve server bu isteği işleyerek cevap verir.
6. Bağlantının sonlandırılması: İşlem tamamlandıktan sonra, client ve server soketlerini kapatır ve bağlantıyı sonlandırır.

Soket programlama, TCP veya UDP gibi iletişim protokollerini kullanarak veri iletişimi sağlar. TCP, güvenilir ve sıralı veri iletimini sağlarken, UDP daha hızlı ancak güvenilirlik gerektirmeyen veri iletimi için kullanılır.

## Internet Protokolleri

Ağ protokolleri, bilgisayarlar arasındaki iletişimi düzenleyen standart kurallar dizisidir. TCP, kayıpsız veri iletimi sağlar ve verinin doğruluğunu kontrol ederken, UDP bağlantısız veri iletimi sağlar.

TCP ve UDP arasındaki farklar şunlardır:

- TCP bağlantı tabanlıdır, UDP bağlantı tabanlı değildir.
- TCP'de akış kontrolü vardır, UDP'de ise yoktur.
- TCP başlığı 20 bayttır, UDP başlığı ise 8 bayttır.
- TCP, UDP'den daha yavaştır ancak veri iletiminde daha güvenilirdir.

## Soketler ve Çeşitleri

Soketler, iletişimde kullanılan temel araçlardır. Stream soketler (SOCK_STREAM), güvenilir veri iletimi için kullanılırken, Datagram soketler (SOCK_DGRAM), güvenilirlik veya sıralama gerektirmeyen veri iletimi için kullanılır.

## Kullanılan Tüm Fonksiyonlar

### socket()
Yeni bir soket oluşturmak için kullanılır. AF_INET, IPv4 adres ailesini temsil ederken, SOCK_STREAM TCP soketi oluşturulacağını belirtir.

### bind()
Bir soketi belirli bir adres ve port numarasına bağlamak için kullanılır.

### listen()
Bir soketi belirli bağlantı taleplerini dinlemek için kullanılan sokete dönüştürür.

### accept()
Yeni bir client bağlantısını kabul etmek için kullanılır.

### poll()
Çoklu soket girişini aynı anda takip etmek ve olayları yönetmek için kullanılır.

## Netcat Nedir?

Netcat, ağ iletişimi için kullanılan bir araçtır. Hem bir server hem de bir client olarak çalışabilir. Port dinlemek, veri göndermek veya almak gibi işlemleri gerçekleştirebilir.

## Komutlar

- **Join**: Bir kanala katılmak için kullanılır.
- **Kick**: Kullanıcıyı bir kanaldan çıkarmak için kullanılır.
- **List**: Kanalların bir listesini almak için kullanılır.
- **Nick**: Bir kullanıcı adı belirtmek veya değiştirmek için kullanılır.
- **Notice**: Kullanıcılara veya kanallara bildirim göndermek için kullanılır.
- **Pass**: Bağlantı şifresi ayarlamak için kullanılır.
- **Ping**: Bağlantının sağlıklı olup olmadığını kontrol etmek için kullanılır.
- **Pong**: Bağlantının sağlıklı olduğunu belirtmek için kullanılır.
- **Privmsg**: Kullanıcılar arasında özel mesaj göndermek için kullanılır.
- **Quit**: Bağlantıyı sonlandırmak için kullanılır.
- **User**: Bir kullanıcı adı belirtmek için kullanılır.
