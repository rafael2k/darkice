CPP=g++

CPPFLAGS=

all: darkice

OBJECTS = main.o\
          Exception.o\
          DarkIce.o\
          Util.o\
          FileSource.o\
          PipeSource.o\
          OssDspSource.o\
          FileSink.o\
          PipeSink.o\
          BufferedSink.o\
          TcpSocket.o\
          CastSink.o\
          IceCast.o\
          Connector.o\
          ExternalEncoder.o\
          LameEncoder.o

darkice: $(OBJECTS)
	$(CPP) $(OBJECTS) -o darkice

clean:
	rm -f $(OBJECTS) darkice

env:
	ls *.h *.cpp > files
	rm -f tags
	ctags -L files -o tags

