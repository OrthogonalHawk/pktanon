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

#ifndef __ANON_FACTORY_H
#define __ANON_FACTORY_H

#include "Configuration.h"
#include "Utils.h"
#include "AnonBytewiseHashSha1.h"
#include "AnonContinuousChar.h"
#include "AnonConstOverwrite.h"
#include "AnonHashSha1.h"
#include "AnonIdentity.h"
#include "AnonPrimitive.h"
#include "AnonRandomize.h"
#include "AnonShorten.h"
#include "AnonHashHmacSha1.h"
#include "AnonBytewiseHashHmacSha1.h"
#include "AnonShuffle.h"
#include "AnonWhitenoise.h"
#include "AnonBroadcastHandler.h"
#include "AnonCryptoPan.h"

#include <string>

using std::string;

class AnonFactory {

public:
								AnonFactory					();
								~AnonFactory				();	

								AnonPrimitive*				create		(Triggerconf::ATTRIBUTE_MAP attrmap);

};

#endif // __ANON_FACTORY_H

