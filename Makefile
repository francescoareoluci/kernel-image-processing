#
# Macros
#
IMG_LDFLAG	= -lpng -pthread
LDFLAGS 	= $(IMG_LDFLAG) -lm

CC		= g++
CFLAGS		= -O2 -Wall -std=c++11

CPP_SRCS	= kernel.cpp \
		  image.cpp \
                  main.cpp

CPP_HDRS	= kernel.h \
		  image.h \

CPP_OBJS	= $(CPP_SRCS:.cpp=.o)
TARGET		= kernel_convolution

CPP_DEPS	= $(CPP_SRCS:.cpp=.d)

#
# Suffix rules
#
.SUFFIXES: .cpp
.cpp.o:
	$(CC) $(CFLAGS)  -c $<

#
# Generating the target
#
all: $(TARGET)

#
# Linking the execution file
#
$(TARGET) : $(CPP_OBJS) 
	$(CC) -o $@ $(CPP_OBJS) $(LDFLAGS)

#
# Cleaning the files
#
clean:
	rm -f $(CPP_OBJS) $(CPP_DEPS) $(TARGET) *~
