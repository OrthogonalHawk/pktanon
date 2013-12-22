bin_PROGRAMS 	 = pktanon
AM_CPPFLAGS      = -DLINUX -D_LINUX -Isrc -D_REENTRANT -D_FILE_OFFSET_BITS=64 -pthread
AM_LDFLAGS       = -lpthread -lxerces-c 
EXTRA_DIST       = profiles/*.xml
pktanon_SOURCES  =

# hook to remove all .svn files before rolling the tarball
dist-hook:
	find $(distdir) -name '.[a-zA-Z0-9]*' | xargs rm -rf
	
pktanon_SOURCES += \
  src/main.cpp \
  src/Configuration.cpp \
  src/Configuration.h \
  src/triggerconf.cpp \
  src/triggerconf.h \
  src/PcapFile.cpp \
  src/PcapFile.h \
  src/RandomNumberGenerator.cpp \
  src/RandomNumberGenerator.h \
  src/StatisticEngine.cpp \
  src/StatisticEngine.h \
  src/Stream.cpp \
  src/Stream.h \
  src/BufferedFile.cpp \
  src/BufferedFile.h \
  src/Transformer.cpp \
  src/Transformer.h \
  src/Timer.cpp \
  src/Timer.h \
  src/Utils.cpp \
  src/Utils.h 

pktanon_SOURCES += \
  src/packets/ArpPacket.cpp \
  src/packets/ArpPacket.h \
  src/packets/EthernetPacket.cpp \
  src/packets/EthernetPacket.h \
  src/packets/Frame.cpp \
  src/packets/Frame.h \
  src/packets/IcmpPacket.cpp \
  src/packets/IcmpPacket.h \
  src/packets/IpPacket.cpp \
  src/packets/IpPacket.h \
  src/packets/Ipv6Packet.cpp \
  src/packets/Ipv6Packet.h \
  src/packets/Packet.cpp \
  src/packets/Packet.h \
  src/packets/PayloadPacket.cpp \
  src/packets/PayloadPacket.h \
  src/packets/TcpPacket.cpp \
  src/packets/TcpPacket.h \
  src/packets/UdpPacket.cpp \
  src/packets/UdpPacket.h \
  src/packets/PacketFactory.cpp \
  src/packets/PacketFactory.h \
  src/packets/VlanPacket.cpp \
  src/packets/VlanPacket.h

#pktanon_SOURCES += \
#  src/benchmark/AnonPrimitiveBenchmark.cpp \
#  src/benchmark/AnonPrimitiveBenchmark.h

#pktanon_SOURCES += \
#  src/anonprimitives/patricia/Patricia.cpp \
#  src/anonprimitives/patricia/Patricia.h

pktanon_SOURCES += \
  src/anonprimitives/hmacsha1/sha1.cpp \
  src/anonprimitives/hmacsha1/sha1.h \
  src/anonprimitives/hmacsha1/sha1_hmac.cpp \
  src/anonprimitives/hmacsha1/sha1_hmac.h \
  src/anonprimitives/cryptopan/rijndael.cpp \
  src/anonprimitives/cryptopan/rijndael.h \
  src/anonprimitives/cryptopan/panonymizer.cpp \
  src/anonprimitives/cryptopan/panonymizer.h \
  src/anonprimitives/AnonBroadcastHandler.cpp \
  src/anonprimitives/AnonBroadcastHandler.h \
  src/anonprimitives/AnonBytewise.cpp \
  src/anonprimitives/AnonBytewise.h \
  src/anonprimitives/AnonBytewiseHashHmacSha1.cpp \
  src/anonprimitives/AnonBytewiseHashHmacSha1.h \
  src/anonprimitives/AnonBytewiseHashSha1.cpp \
  src/anonprimitives/AnonBytewiseHashSha1.h \
  src/anonprimitives/AnonConstOverwrite.cpp \
  src/anonprimitives/AnonConstOverwrite.h \
  src/anonprimitives/AnonContinuousChar.cpp \
  src/anonprimitives/AnonContinuousChar.h \
  src/anonprimitives/AnonFactory.cpp \
  src/anonprimitives/AnonFactory.h \
  src/anonprimitives/AnonHashHmacSha1.cpp \
  src/anonprimitives/AnonHashHmacSha1.h \
  src/anonprimitives/AnonHashSha1.cpp \
  src/anonprimitives/AnonHashSha1.h \
  src/anonprimitives/AnonIdentity.cpp \
  src/anonprimitives/AnonIdentity.h \
  src/anonprimitives/AnonPrimitive.cpp \
  src/anonprimitives/AnonPrimitive.h \
  src/anonprimitives/AnonRandomize.cpp \
  src/anonprimitives/AnonRandomize.h \
  src/anonprimitives/AnonShorten.cpp \
  src/anonprimitives/AnonShorten.h \
  src/anonprimitives/AnonShuffle.cpp \
  src/anonprimitives/AnonShuffle.h \
  src/anonprimitives/AnonWhitenoise.cpp \
  src/anonprimitives/AnonWhitenoise.h \
  src/anonprimitives/AnonCryptoPan.cpp \
  src/anonprimitives/AnonCryptoPan.h
