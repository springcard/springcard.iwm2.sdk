JFLAGS = -g $(LIBS)
JC = I:\builder\JDK7u51_x64\bin\javac.exe

.SUFFIXES: .java .class .jar


SRCS:=\
	com/springcard/fkgtcp/Listener.java \
	com/springcard/fkgtcp/StringBytes.java \
  com/springcard/fkgtcp/PrintArray.java \
  com/springcard/fkgtcp/SCrypto.java \
	com/springcard/fkgtcp/Connector.java \
	com/springcard/fkgtcp/FunkyGateConnector.java	\
	com/springcard/fkgtcp/HandyDrummerConnector.java \
	fkgtcpDemo.java
  
OBJS:=$(patsubst %.java,%.class,$(SRCS))
	
all: $(OBJS)
  
clean:
	$(RM) *.class

%.class: %.java
	$(JC) $(JFLAGS) $*.java
