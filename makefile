
preprocessor: CurlTraceServer.o ./xmlWriter/ExportManifestXml.o ./xmlWriter/XmlWriter.o
	g++ CurlTraceServer.o ./xmlWriter/ExportManifestXml.o ./xmlWriter/XmlWriter.o -o preprocessor -ljsoncpp -lcurl


CurlTraceServer.o: CurlTraceServer.cpp config.h
	g++ -c CurlTraceServer.cpp 

all: 
	+$(MAKE) -C xmlWriter

clean: 
	rm *.o preprocessor
	rm ./xmlWriter/*.o 