# Derleyici
CC := c++

# Derleme seçenekleri
CFLAGS := -Wall -Wextra -Werror -std=c++98 -g 

# Derleme hedefi
TARGET := ircserv
SENDER_LOG := sender.log
# Kaynak dosyaları
SOURCES := src/main.cpp src/server.cpp  src/getMessage.cpp src/utils.cpp src/users.cpp src/channel.cpp src/error.cpp

# Nesne dosyaları
OBJECTS := $(SOURCES:.cpp=.o)

# Kural: Tüm hedef
all: $(TARGET)

# Kural: Nesne dosyalarını oluştur
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Kural: Programı derle ve oluşturulan dosyayı taşı
$(TARGET): $(OBJECTS)
	$(CC) $^ $(CFLAGS) -o $@

# Temizleme işlemi
clean:
	rm -f $(OBJECTS) 

fclean: clean
	rm -f $(TARGET) $(SENDER_LOG)

re : clean all
# PHONY hedefleri
.PHONY: all clean re