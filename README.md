# Açıklama
Bu projede pick to light cihazı için gömülü yazılım geliştirilmiştir. Bilgisayardan gelen verileri USB haberleşmesi ile okuyan 1 adet master STM32F1 işlemci aldığı mesajı anlamlandırarak, 
I2C haberleşmesi ile 4 adet slave STM32F1 işlemciye bu mesajı iletmektedir. Her bir slave mikroişlemci 70 adet led kontrolü yapabilmekte ve toplamda 280 adet led kontrol edilmektedir.    
**Kullanılan Program** <br/> -STM32CubeIde <br/>
