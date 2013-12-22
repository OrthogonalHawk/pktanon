//
// Copyright (C) 2008-2011 Institute of Telematics, Karlsruhe Institute of Technology (KIT)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include "Transformer.h"

Transformer::Transformer()	
{
}

Transformer::~Transformer(void)
{
}

Packet* Transformer::getTransformedPacket(Packet& inpacket)
{
	Packet* newpkt = NULL;

	newpkt = PacketFactory::instance()->createPacket (inpacket.getProtocol ());

	switch (inpacket.getProtocol()) {
		case Packet::PROTO_ETHERNET:
			{
				transformPacket ((EthernetPacket&)inpacket, (EthernetPacket&)*newpkt);
				break;
			}
		case Packet::PROTO_ARP:
			{
				transformPacket ((ArpPacket&)inpacket, (ArpPacket&)*newpkt);
				break;
			}
		case Packet::PROTO_ICMP:
			{
				transformPacket ((IcmpPacket&)inpacket, (IcmpPacket&)*newpkt);
				break;
			}
		case Packet::PROTO_IP:
			{
				transformPacket ((IpPacket&)inpacket, (IpPacket&)*newpkt);
				break;
			}
		case Packet::PROTO_IPV6:
			{
				transformPacket ((Ipv6Packet&)inpacket, (Ipv6Packet&)*newpkt);
				break;
			}
		case Packet::PROTO_TCP:
			{
				transformPacket ((TcpPacket&)inpacket, (TcpPacket&)*newpkt);
				break;
			}
		case Packet::PROTO_UDP:
			{
				transformPacket ((UdpPacket&)inpacket, (UdpPacket&)*newpkt);
				break;
			}
		case Packet::PROTO_DATA_PAYLOAD:
			{
				transformPacket ((PayloadPacket&)inpacket, (PayloadPacket&)*newpkt);
				break;
			}
		case Packet::PROTO_VLAN:
			{
				transformPacket ((VlanPacket&)inpacket, (VlanPacket&)*newpkt);
				break;
			}
		case Packet::PROTO_NONE:
		default:
			{
				return NULL;
			}
	} // switch (inpacket.protocol)
	
	newpkt->setChecksumgood		(inpacket.getChecksumgood ());

	if (inpacket.getNextPacket() != NULL) {
		Packet* innertrans		= getTransformedPacket (*inpacket.getNextPacket());
		newpkt->setNextPacket	(innertrans);
		newpkt->setNextProtocol (innertrans->getProtocol ());
	}

	return newpkt;
}

void Transformer::transformPacket (EthernetPacket& inpkt, EthernetPacket& ret)
{
	MAC_ADDR sourcemac								= inpkt.getSourceMac ();
	EthernetPacket::anonSourceMac->anonimizeBuffer	(&sourcemac, sizeof (MAC_ADDR));
	ret.setSourceMac								(sourcemac);

	MAC_ADDR destmac								= inpkt.getDestMac ();
	EthernetPacket::anonDestMac->anonimizeBuffer	(&destmac, sizeof (MAC_ADDR));
	ret.setDestMac									(destmac);

	unsigned short type								= inpkt.getType ();
	EthernetPacket::anonType->anonimizeBuffer		(&type, sizeof (unsigned short));
	ret.setType										(type);
}

void Transformer::transformPacket (ArpPacket& inpkt, ArpPacket& ret)
{
	unsigned short hwtype							= inpkt.getHardwaretype ();	
	ArpPacket::anonHardwaretype->anonimizeBuffer	(&hwtype, sizeof (unsigned short));
	ret.setHardwaretype								(hwtype);

	unsigned short prototype						= inpkt.getProtocoltype ();
	ArpPacket::anonProtocoltype->anonimizeBuffer	(&prototype, sizeof (unsigned short));
	ret.setProtocoltype								(prototype);

	unsigned char hwaddrlen							= inpkt.getHardwareaddrlen ();
	ArpPacket::anonHardwareaddrlen->anonimizeBuffer	(&hwaddrlen, sizeof (unsigned char));
	ret.setHardwareaddrlen							(hwaddrlen);

	unsigned char protoaddrlen						= inpkt.getProtocoladdrlen ();
	ArpPacket::anonProtoaddrlen->anonimizeBuffer	(&protoaddrlen, sizeof (unsigned char));
	ret.setProtocoladdrlen							(protoaddrlen);

	unsigned short opcode							= inpkt.getOpcode ();
	ArpPacket::anonOpcode->anonimizeBuffer			(&opcode, sizeof (unsigned short));
	ret.setOpcode									(opcode);

	MAC_ADDR sourcemac								= inpkt.getSourcemac ();
	ArpPacket::anonSourcemac->anonimizeBuffer		(&sourcemac, sizeof (MAC_ADDR));
	ret.setSourcemac								(sourcemac);

	IP_ADDR	sourceip								= inpkt.getSourceip ();
	ArpPacket::anonSourceip->anonimizeBuffer		(&sourceip, sizeof (IP_ADDR));
	ret.setSourceip									(sourceip);

	MAC_ADDR destmac								= inpkt.getDestmac ();
	ArpPacket::anonDestmac->anonimizeBuffer			(&destmac, sizeof (MAC_ADDR));
	ret.setDestmac									(destmac);

	IP_ADDR	destip									= inpkt.getDestip	();
	ArpPacket::anonDestip->anonimizeBuffer			(&destip, sizeof (IP_ADDR));
	ret.setDestip									(destip);
}

void Transformer::transformPacket (IpPacket& inpkt, IpPacket& ret)
{
	ret.setVersion									(inpkt.getVersion			()  );
	ret.setHeaderlength								(inpkt.getHeaderlength		()  );
	ret.setTotallen									(inpkt.getTotallen			()  );
	ret.setProtocol									(inpkt.getProtocol			()  );
	ret.setChecksum									(inpkt.getChecksum			()  );

	unsigned char tos								= inpkt.getTos ();
	IpPacket::anonTos->anonimizeBuffer				(&tos, sizeof (unsigned char));
	ret.setTos										(tos);

	unsigned short ident							= inpkt.getIdentification ();
	IpPacket::anonIdentifier->anonimizeBuffer		(&ident, sizeof (unsigned short));
	ret.setIdentification							(ident);
	
	unsigned short flags							= inpkt.getFlags ();
	IpPacket::anonFlags->anonimizeBuffer			(&flags, sizeof (unsigned short));
	ret.setFlags									(flags);

	unsigned short fragoffset						= inpkt.getFragoffset ();
	IpPacket::anonFragoffset->anonimizeBuffer		(&fragoffset, sizeof (unsigned short));
	ret.setFragoffset								(fragoffset);

	unsigned char ttl								= inpkt.getTtl ();
	IpPacket::anonTtl->anonimizeBuffer				(&ttl, sizeof (unsigned char));
	ret.setTtl										(ttl);

	IP_ADDR sourceip								= inpkt.getSourceip ();
	IpPacket::anonSourceip->anonimizeBuffer			(&sourceip, sizeof (IP_ADDR));
	ret.setSourceip									(sourceip);

	IP_ADDR destip									= inpkt.getDestip ();
	IpPacket::anonDestip->anonimizeBuffer			(&destip, sizeof (IP_ADDR));
	ret.setDestip									(destip);

	Packet::PAYLOAD_BUFFER options							= inpkt.getOptions ();
	options.size = IpPacket::anonOptions->anonimizeBuffer	(options.buf, options.size);
	ret.setOptions											(options.buf, options.size);
	options.destroy											();
}

void Transformer::transformPacket (IcmpPacket& inpkt, IcmpPacket& ret)
{
	unsigned char type							= inpkt.getType ();
	IcmpPacket::anonType->anonimizeBuffer		(&type, sizeof (unsigned char));
	ret.setType									(type);

	unsigned char code							= inpkt.getCode ();
	IcmpPacket::anonCode->anonimizeBuffer		(&code, sizeof (unsigned char));
	ret.setCode									(code);

	unsigned long misc							= inpkt.getMisc ();
	IcmpPacket::anonMisc->anonimizeBuffer		(&misc, sizeof (unsigned long));
	ret.setMisc									(misc);
}

void Transformer::transformPacket (TcpPacket& inpkt, TcpPacket& ret)
{
	ret.setHeaderlength							(inpkt.getHeaderlength	()	);
	ret.setChecksum								(inpkt.getChecksum		()	);

	unsigned short sourceport					= inpkt.getSourceport ();
	TcpPacket::anonSourceport->anonimizeBuffer	(&sourceport, sizeof (unsigned short));
	ret.setSourceport							(sourceport);

	unsigned short destport						= inpkt.getDestport ();
	TcpPacket::anonDestport->anonimizeBuffer	(&destport, sizeof (unsigned short));
	ret.setDestport								(destport);

	unsigned long sequencenum					= inpkt.getSequencenum ();
	TcpPacket::anonSeqnum->anonimizeBuffer		(&sequencenum, sizeof (unsigned long));
	ret.setSequencenum							(sequencenum);
	
	unsigned long acknum						= inpkt.getAcknum ();
	TcpPacket::anonAcknum->anonimizeBuffer		(&acknum, sizeof (unsigned long));
	ret.setAcknum								(acknum);

	unsigned short flags						= inpkt.getFlags ();
	TcpPacket::anonFlags->anonimizeBuffer		(&flags, sizeof (unsigned short));
	ret.setFlags								(flags);

	unsigned short windowsize					= inpkt.getWindowsize ();
	TcpPacket::anonWindowsize->anonimizeBuffer	(&windowsize, sizeof (unsigned short));
	ret.setWindowsize							(windowsize);

	unsigned short urgentpnt					= inpkt.getUrgentpointer ();
	TcpPacket::anonUrgentpnt->anonimizeBuffer	(&urgentpnt, sizeof (unsigned short));
	ret.setUrgentpointer						(urgentpnt);

	Packet::PAYLOAD_BUFFER options							= inpkt.getOptions ();
	options.size = TcpPacket::anonOptions->anonimizeBuffer	(options.buf, options.size);
	ret.setOptions											(options.buf, options.size);
	options.destroy											();
}

void Transformer::transformPacket (UdpPacket& inpkt, UdpPacket& ret)
{
	ret.setLen									(inpkt.getLen		()	);
	ret.setChecksum								(inpkt.getChecksum	()	);
	
	unsigned short destport						= inpkt.getDestport ();
	UdpPacket::anonDestport->anonimizeBuffer	(&destport, sizeof (unsigned short));
	ret.setDestport								(destport);

	unsigned short sourceport					= inpkt.getSourceport ();
	UdpPacket::anonSourceport->anonimizeBuffer	(&sourceport, sizeof (unsigned short));
	ret.setSourceport							(sourceport);
}

void Transformer::transformPacket (VlanPacket& inpkt, VlanPacket& ret)
{
	uint8_t priority 							= inpkt.getPriority();
	VlanPacket::anonPriority->anonimizeBuffer	(&priority, sizeof(uint8_t));
	ret.setPriority								(priority);

	bool cfi 									= inpkt.getCanonicalFormatIdentifier();
	VlanPacket::anonCfi->anonimizeBuffer		(&cfi, sizeof(bool));
	ret.setCanonicalFormatIdentifier			(cfi);

	uint16_t vlanid 							= inpkt.getVlanIdentifier();
	VlanPacket::anonVlanId->anonimizeBuffer		(&vlanid, sizeof(uint16_t));
	ret.setVlanIdentifier						(vlanid);

	uint16_t nextprotocol 						= inpkt.getType();
	VlanPacket::anonType->anonimizeBuffer		(&nextprotocol, sizeof(uint16_t));
	ret.setType									(nextprotocol);
}

void Transformer::transformPacket (PayloadPacket& inpkt, PayloadPacket& ret)
{
	Packet::PAYLOAD_BUFFER payload								= inpkt.getData();
	payload.size = PayloadPacket::anonPayload->anonimizeBuffer	(payload.buf, payload.size);
	ret.setData													(payload.buf, payload.size);
	payload.destroy												();
}

void Transformer::transformPacket (Ipv6Packet& inpkt, Ipv6Packet& ret)
{
	ret.setVersion											(inpkt.getVersion ());
	ret.setPayloadlen										(inpkt.getPayloadlen ());
	ret.setNextheader										(inpkt.getNextheader ());

	unsigned char trafficclass								= inpkt.getTrafficclass ();
	Ipv6Packet::anonTrafficclass->anonimizeBuffer			(&trafficclass, sizeof (unsigned char));
	ret.setTrafficclass										(trafficclass);

	unsigned int flowlabel									= inpkt.getFlowlabel ();
	Ipv6Packet::anonFlowlabel->anonimizeBuffer				(&flowlabel, sizeof (unsigned int ));
	ret.setFlowlabel										(flowlabel);

	unsigned char hoplimit									= inpkt.getHoplimit ();
	Ipv6Packet::anonHoplimit->anonimizeBuffer				(&hoplimit, sizeof (unsigned char));
	ret.setHoplimit											(hoplimit);

	IPV6_ADDR sourceaddr									= inpkt.getSourceaddr ();
	Ipv6Packet::anonSourceaddr->anonimizeBuffer				(&sourceaddr, sizeof (IPV6_ADDR));
	ret.setSourceaddr										(sourceaddr);

	IPV6_ADDR destaddr										= inpkt.getDestaddr ();
	Ipv6Packet::anonDestaddr->anonimizeBuffer				(&destaddr, sizeof (IPV6_ADDR));
	ret.setDestaddr											(destaddr);
}

//
//
// TODO: the rest of this file must be patched into the
// anonymizer classes. If this anon primitive is actually needed
//
//

/*

void Transformer::hashIpCidr (IP_ADDR& addr)
{
	//
	// this should hopefully apply to all who have not 
	// blown their mind while implementing their compiler ...
	// byte is 8 bit, int and unsigned int are 32 bit
	//

	assert (sizeof (unsigned int) == IP_ADDR_LEN);

	//
	// first we find out the number of bits in the network and host parts
	//

	Patricia::PNODE node = patriciaTree.find (addr);

	unsigned short networkbits	= 0;
	unsigned short hostbits		= IP_ADDR_LEN * 8;

	if (node != NULL) {
		networkbits = node->count;
		hostbits	= (IP_ADDR_LEN * 8) - networkbits;
	}

	//
	// now we hash the network part and the host part
	// independently and concat them.
	// as we can only hash bytes, we hash the next higher number
	// of bits to be bytes, then we remove the padding bits and concat
	//

	// which bytes and bits are net-/hostpart

	unsigned short networkbytes = networkbits	% 8 == 0 ? networkbits	/ 8 : (networkbits	/ 8) + 1;
	unsigned short hostbytes	= hostbits		% 8 == 0 ? hostbits		/ 8 : (hostbits		/ 8) + 1;

	assert ((networkbytes + hostbytes >= 4) && (networkbytes + hostbytes <= 5));
	assert (networkbits + hostbits == IP_ADDR_LEN * 8);

	unsigned int networkip;
	unsigned int hostip;

	// create network and host bitmask 

	unsigned int networkmask	= 0;
	unsigned int hostmask		= 0;

	for (int i=IP_ADDR_LEN*8-1; i>=IP_ADDR_LEN*8-networkbits; i--) 
		networkmask |= (1 << i);	
		
	for (int i=hostbits-1; i>=0; i--) 
		hostmask |= (1 << i);	

	assert ((networkmask | hostmask) == 0xFFFFFFFF);

	// apply the network and host bitmask to the ip

	memcpy (&networkip, &addr, sizeof (unsigned int));
	memcpy (&hostip,	&addr, sizeof (unsigned int));

	networkip	&= networkmask;
	hostip		&= hostmask;
	
	assert ((networkip & hostip) == 0); 

#ifdef WIN32
	// have to correct byte order
	networkip	= swap32 (networkip	);
	hostip		= swap32 (hostip	);
#endif
	
	//
	// if we already have the hashed network part we use it
	// if not, we create it and save it
	//

	if (node->networkip != NULL) {

		memcpy (&networkip, &node->networkip, sizeof (unsigned int));

	} else {	
	
		hash ((unsigned char*)&networkip,	networkbytes);

#ifdef WIN32
		// have to recorrect byte order
		networkip	= swap32 (networkip	);
#endif

		node->networkip = (PIP_ADDR) malloc (sizeof (IP_ADDR));
		memcpy (node->networkip, &networkip, sizeof (IP_ADDR));

	} // if (node->networkip != NULL) 

	hash ((unsigned char*)&hostip,		hostbytes	);

#ifdef WIN32
	// have to recorrect byte order
	hostip = swap32 (hostip);
#endif

	unsigned int endaddr = (networkip & networkmask) | (hostip & hostmask);

	memcpy (&addr, &endaddr, IP_ADDR_LEN);
}

void Transformer::config_xxxx()
{
	//
	// if we are hashing cidr-like we read the routing info
	// now. the file _must_ be called routes.txt
	// 
	
	ifstream routes;
	routes.open ("routes.txt", ios_base::in);

	if (! routes.is_open ()) {
		cerr << "routes.txt not found" << std::endl;
		exit (1);
	}

	do {
		
		//
		// read each line in the file
		// each line has a format like xxx.xxx.xxx.xxx/yy
		//

		char buf [32];
		routes.getline ((char*) &buf, 32);

		string iproute = buf;	
		
		// trim the string
		iproute.erase (0, iproute.find_first_not_of( " \n\r\t" ));
		iproute.erase (iproute.find_last_not_of( " \n\r\t") + 1 );

		// split into ip and mask
		string	ip		= iproute.substr (0, iproute.find_first_of ('/'));
		string	mask	= iproute.substr (iproute.find_first_of ('/') + 1);

		// convert string ip to ip address
		IP_ADDR addr	= {{0}};
		int		start	= 0;
		int		end		= 0;
		string	byte	= "";

		for (int i=0; i<IP_ADDR_LEN; i++, start = end + 1) {
			end			= (int) ip.find_first_of ('.', start);
			byte		= ip.substr (start, end - start);
			addr.x[i]	= (unsigned char) strtoul (byte.c_str(), NULL, 10);
		}

		//cerr << addr.toString () << "/" << mask << std::endl;

		// insert into patricia tree
		patriciaTree.insert (addr, (unsigned short) strtoul (mask.c_str (), NULL, 10));

	} while (routes.good () && routes.eof () == false); 

}

*/
