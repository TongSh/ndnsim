/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include "interest.hpp"
#include "util/random.hpp"
#include "data.hpp"

#include <cstring>
#include <sstream>

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Interest>));
BOOST_CONCEPT_ASSERT((WireEncodable<Interest>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Interest>));
BOOST_CONCEPT_ASSERT((WireDecodable<Interest>));
static_assert(std::is_base_of<tlv::Error, Interest::Error>::value,
              "Interest::Error must inherit from tlv::Error");

Interest::Interest(const Name& name, time::milliseconds interestLifetime)
  : m_name(name)
  , m_interestLifetime(interestLifetime)
{
  if (interestLifetime < time::milliseconds::zero()) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("InterestLifetime must be >= 0"));
  }
}

Interest::Interest(const Block& wire)
{
  wireDecode(wire);
}

// ---- encode and decode ----

template<encoding::Tag TAG>
size_t
Interest::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  // Interest ::= INTEREST-TYPE TLV-LENGTH
  //                Name
  //                Selectors?
  //                Nonce
  //                InterestLifetime?
  //                ForwardingHint?

  // (reverse encoding)

  // ForwardingHint
  if (m_forwardingHint.size() > 0) {
    totalLength += m_forwardingHint.wireEncode(encoder);
  }

  // InterestLifetime
  if (getInterestLifetime() != DEFAULT_INTEREST_LIFETIME) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                                                  tlv::InterestLifetime,
                                                  getInterestLifetime().count());
  }

  // Nonce
  uint32_t nonce = this->getNonce(); // assigns random Nonce if needed
  totalLength += encoder.prependByteArray(reinterpret_cast<uint8_t*>(&nonce), sizeof(nonce));
  totalLength += encoder.prependVarNumber(sizeof(nonce));
  totalLength += encoder.prependVarNumber(tlv::Nonce);

  // Selectors
  if (hasSelectors()) {
    totalLength += getSelectors().wireEncode(encoder);
  }

  // Name
  totalLength += getName().wireEncode(encoder);

  // @Tong
    totalLength += encoder.prependBlock(m_hop_posx);
    totalLength += encoder.prependBlock(m_hop_posy);
    totalLength += encoder.prependBlock(m_hop_dir);
    totalLength += encoder.prependBlock(m_hop_id);
    totalLength += encoder.prependBlock(m_hop_velocityX);
    totalLength += encoder.prependBlock(m_hop_velocityY);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::Interest);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(Interest);

const Block&
Interest::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  const_cast<Interest*>(this)->wireDecode(buffer.block());
  return m_wire;
}

void
Interest::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  if (m_wire.type() != tlv::Interest)
    BOOST_THROW_EXCEPTION(Error("Unexpected TLV number when decoding Interest"));

  // Name
  m_name.wireDecode(m_wire.get(tlv::Name));

  // Selectors
  Block::element_const_iterator val = m_wire.find(tlv::Selectors);
  if (val != m_wire.elements_end()) {
    m_selectors.wireDecode(*val);
  }
  else
    m_selectors = Selectors();

  // Nonce
  val = m_wire.find(tlv::Nonce);
  if (val == m_wire.elements_end()) {
    BOOST_THROW_EXCEPTION(Error("Nonce element is missing"));
  }
  uint32_t nonce = 0;
  if (val->value_size() != sizeof(nonce)) {
    BOOST_THROW_EXCEPTION(Error("Nonce element is malformed"));
  }
  std::memcpy(&nonce, val->value(), sizeof(nonce));
  m_nonce = nonce;


  // Tong
    val = m_wire.find(tlv::LastHop);
    if (val != m_wire.elements_end())
  	  m_hop_id = *val;
    val = m_wire.find(tlv::PosX);
    if (val != m_wire.elements_end())
  	  m_hop_posx = *val;
    val = m_wire.find(tlv::PosY);
    if (val != m_wire.elements_end())
  	  m_hop_posy = *val;
    val = m_wire.find(tlv::Direction);
    if (val != m_wire.elements_end())
  	  m_hop_dir = *val;
    val = m_wire.find(tlv::InterestVelocityX);
    if (val != m_wire.elements_end())
  	  m_hop_velocityX = *val;
    val = m_wire.find(tlv::InterestVelocityY);
    if (val != m_wire.elements_end())
  	  m_hop_velocityY = *val;

  // InterestLifetime
  val = m_wire.find(tlv::InterestLifetime);
  if (val != m_wire.elements_end()) {
    m_interestLifetime = time::milliseconds(readNonNegativeInteger(*val));
  }
  else {
    m_interestLifetime = DEFAULT_INTEREST_LIFETIME;
  }

  // ForwardingHint
  val = m_wire.find(tlv::ForwardingHint);
  if (val != m_wire.elements_end()) {
    m_forwardingHint.wireDecode(*val, false);
  }
  else {
    m_forwardingHint = DelegationList();
  }
}

std::string
Interest::toUri() const
{
  std::ostringstream os;
  os << *this;
  return os.str();
}

// ---- matching ----

bool
Interest::matchesName(const Name& name) const
{
  if (name.size() < m_name.size())
    return false;

  if (!m_name.isPrefixOf(name))
    return false;

  if (getMinSuffixComponents() >= 0 &&
      // name must include implicit digest
      !(name.size() - m_name.size() >= static_cast<size_t>(getMinSuffixComponents())))
    return false;

  if (getMaxSuffixComponents() >= 0 &&
      // name must include implicit digest
      !(name.size() - m_name.size() <= static_cast<size_t>(getMaxSuffixComponents())))
    return false;

  if (!getExclude().empty() &&
      name.size() > m_name.size() &&
      getExclude().isExcluded(name[m_name.size()]))
    return false;

  return true;
}

bool
Interest::matchesData(const Data& data) const
{
  size_t interestNameLength = m_name.size();
  const Name& dataName = data.getName();
  size_t fullNameLength = dataName.size() + 1;

  // check MinSuffixComponents
  bool hasMinSuffixComponents = getMinSuffixComponents() >= 0;
  size_t minSuffixComponents = hasMinSuffixComponents ?
                               static_cast<size_t>(getMinSuffixComponents()) : 0;
  if (!(interestNameLength + minSuffixComponents <= fullNameLength))
    return false;

  // check MaxSuffixComponents
  bool hasMaxSuffixComponents = getMaxSuffixComponents() >= 0;
  if (hasMaxSuffixComponents &&
      !(interestNameLength + getMaxSuffixComponents() >= fullNameLength))
    return false;

  // check prefix
  if (interestNameLength == fullNameLength) {
    if (m_name.get(-1).isImplicitSha256Digest()) {
      if (m_name != data.getFullName())
        return false;
    }
    else {
      // Interest Name is same length as Data full Name, but last component isn't digest
      // so there's no possibility of matching
      return false;
    }
  }
  else {
    // Interest Name is a strict prefix of Data full Name
    if (!m_name.isPrefixOf(dataName))
      return false;
  }

  // check Exclude
  // Exclude won't be violated if Interest Name is same as Data full Name
  if (!getExclude().empty() && fullNameLength > interestNameLength) {
    if (interestNameLength == fullNameLength - 1) {
      // component to exclude is the digest
      if (getExclude().isExcluded(data.getFullName().get(interestNameLength)))
        return false;
      // There's opportunity to inspect the Exclude filter and determine whether
      // the digest would make a difference.
      // eg. "<NameComponent>AA</NameComponent><Any/>" doesn't exclude any digest -
      //     fullName not needed;
      //     "<Any/><NameComponent>AA</NameComponent>" and
      //     "<Any/><ImplicitSha256DigestComponent>ffffffffffffffffffffffffffffffff
      //      </ImplicitSha256DigestComponent>"
      //     excludes all digests - fullName not needed;
      //     "<Any/><ImplicitSha256DigestComponent>80000000000000000000000000000000
      //      </ImplicitSha256DigestComponent>"
      //     excludes some digests - fullName required
      // But Interests that contain the exact Data Name before digest and also
      // contain Exclude filter is too rare to optimize for, so we request
      // fullName no matter what's in the Exclude filter.
    }
    else {
      // component to exclude is not the digest
      if (getExclude().isExcluded(dataName.get(interestNameLength)))
        return false;
    }
  }

  // check PublisherPublicKeyLocator
  const KeyLocator& publisherPublicKeyLocator = this->getPublisherPublicKeyLocator();
  if (!publisherPublicKeyLocator.empty()) {
    const Signature& signature = data.getSignature();
    const Block& signatureInfo = signature.getInfo();
    Block::element_const_iterator it = signatureInfo.find(tlv::KeyLocator);
    if (it == signatureInfo.elements_end()) {
      return false;
    }
    if (publisherPublicKeyLocator.wireEncode() != *it) {
      return false;
    }
  }

  return true;
}

bool
Interest::matchesInterest(const Interest& other) const
{
  /// @todo #3162 match ForwardingHint field
  return (this->getName() == other.getName() &&
          this->getSelectors() == other.getSelectors());
}

// ---- field accessors ----

uint32_t
Interest::getNonce() const
{
  if (!m_nonce) {
    m_nonce = random::generateWord32();
  }
  return *m_nonce;
}

Interest&
Interest::setNonce(uint32_t nonce)
{
  m_nonce = nonce;
  m_wire.reset();
  return *this;
}

void
Interest::refreshNonce()
{
  if (!hasNonce())
    return;

  uint32_t oldNonce = getNonce();
  uint32_t newNonce = oldNonce;
  while (newNonce == oldNonce)
    newNonce = random::generateWord32();

  setNonce(newNonce);
}

Interest&
Interest::setInterestLifetime(time::milliseconds interestLifetime)
{
  if (interestLifetime < time::milliseconds::zero()) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("InterestLifetime must be >= 0"));
  }
  m_interestLifetime = interestLifetime;
  m_wire.reset();
  return *this;
}

Interest&
Interest::setForwardingHint(const DelegationList& value)
{
  m_forwardingHint = value;
  m_wire.reset();
  return *this;
}

// @Tong
Interest&
Interest::setHopPosx(double x)
{
	// * trans double to int
	uint32_t posX = floor(x*100000);
	if(m_hop_posx.hasWire() && m_hop_posx.value_size() == sizeof(uint32_t))
		{
			std::memcpy(const_cast<uint8_t*>(m_hop_posx.value()),&posX,sizeof(posX));
		}
		else
		{
			m_hop_posx = makeBinaryBlock(tlv::PosX,
					reinterpret_cast<const uint8_t*>(&posX),
					sizeof(posX));
			m_wire.reset();
		}
		return *this;
}

double
Interest::getHopPosx() const
{
	uint32_t posX;
	double x;
	if(!m_hop_posx.hasWire())
		const_cast<Interest*>(this)->setHopPosx(6666);
	if(m_hop_posx.value_size() == sizeof(uint32_t))
		posX = *reinterpret_cast<const uint32_t*>(m_hop_posx.value());
	else
	{
		posX = readNonNegativeInteger(m_hop_posx);
	}

	if(posX)
		x = posX/100000.0;
	else
		x = 0.0;
	return x;
}

Interest&
Interest::setHopPosy(double x)
{
	// * trans double to int
	uint32_t posY = floor(x*100000);
	if(m_hop_posy.hasWire() && m_hop_posy.value_size() == sizeof(uint32_t))
		{
			std::memcpy(const_cast<uint8_t*>(m_hop_posy.value()),&posY,sizeof(posY));
		}
		else
		{
			m_hop_posy = makeBinaryBlock(tlv::PosY,
					reinterpret_cast<const uint8_t*>(&posY),
					sizeof(posY));
			m_wire.reset();
		}
		return *this;
}

double
Interest::getHopPosy() const
{
	uint32_t posY;
	double y;
	if(!m_hop_posy.hasWire())
		const_cast<Interest*>(this)->setHopPosy(6666);
	if(m_hop_posy.value_size() == sizeof(uint32_t))
		posY = *reinterpret_cast<const uint32_t*>(m_hop_posy.value());
	else
	{
		posY = readNonNegativeInteger(m_hop_posy);
	}

	if(posY)
		y = posY/100000.0;
	else
		y = 0.0;
	return y;
}

// velocity/////////////////////////////////
Interest&
Interest::setHopVelocityX(double x)
{
	// * trans double to int
	uint32_t velocityX;
	if(x < 0){
		velocityX = (-x)*1000000;
		velocityX = velocityX | 0x80000000;
	}else{
		velocityX = x*1000000;
	}
	if(m_hop_velocityX.hasWire() && m_hop_velocityX.value_size() == sizeof(uint32_t))
		{
			std::memcpy(const_cast<uint8_t*>(m_hop_velocityX.value()),&velocityX,sizeof(velocityX));
		}
		else
		{
			m_hop_velocityX = makeBinaryBlock(tlv::InterestVelocityX,
					reinterpret_cast<const uint8_t*>(&velocityX),
					sizeof(velocityX));
			m_wire.reset();
		}
		return *this;
}

double
Interest::getHopVelocityX() const
{
	uint32_t velocityX;
	double y;
	if(!m_hop_velocityX.hasWire())
		const_cast<Interest*>(this)->setHopVelocityX(6666);
	if(m_hop_velocityX.value_size() == sizeof(uint32_t))
		velocityX = *reinterpret_cast<const uint32_t*>(m_hop_velocityX.value());
	else
	{
		velocityX = readNonNegativeInteger(m_hop_velocityX);
	}

	if(velocityX){
		if(velocityX & 0x80000000){
			y = (velocityX & 0x7fffffff)/1000000.0;
			y = -y;
		}else{
			y = velocityX / 1000000.0;
		}
	}else
		y = 0.0;
	return y;
}

Interest&
Interest::setHopVelocityY(double x)
{
	// * trans double to int
	uint32_t velocityY;
	if(x < 0){
		velocityY = (-x)*1000000;
		velocityY = velocityY | 0x80000000;
	}else{
		velocityY = x*1000000;
	}
	if(m_hop_velocityY.hasWire() && m_hop_velocityY.value_size() == sizeof(uint32_t))
		{
			std::memcpy(const_cast<uint8_t*>(m_hop_velocityY.value()),&velocityY,sizeof(velocityY));
		}
		else
		{
			m_hop_velocityY = makeBinaryBlock(tlv::InterestVelocityY,
					reinterpret_cast<const uint8_t*>(&velocityY),
					sizeof(velocityY));
			m_wire.reset();
		}
		return *this;
}

double
Interest::getHopVelocityY() const
{
	uint32_t velocityY;
	double y;
	if(!m_hop_velocityY.hasWire())
		const_cast<Interest*>(this)->setHopVelocityY(6666);
	if(m_hop_velocityY.value_size() == sizeof(uint32_t))
		velocityY = *reinterpret_cast<const uint32_t*>(m_hop_velocityY.value());
	else
	{
		velocityY = readNonNegativeInteger(m_hop_velocityY);
	}

	if(velocityY){
		if(velocityY & 0x80000000){
			y = (velocityY & 0x7fffffff)/1000000.0;
			y = -y;
		}else{
			y = velocityY / 1000000.0;
		}
	}else
		y = 0.0;
	return y;
}

// Direciton/////////////////////////////
Interest&
Interest::setHopDir(double p_dir)
{
	// * trans double to int
	uint32_t Dir = floor(p_dir*100000);
	if(m_hop_dir.hasWire() && m_hop_dir.value_size() == sizeof(uint32_t))
		{
			std::memcpy(const_cast<uint8_t*>(m_hop_dir.value()),&Dir,sizeof(Dir));
		}
		else
		{
			m_hop_dir = makeBinaryBlock(tlv::Direction,
					reinterpret_cast<const uint8_t*>(&Dir),
					sizeof(Dir));
			m_wire.reset();
		}
		return *this;
}

double
Interest::getHopDir() const
{
	uint32_t dir;
	double d;
	if(!m_hop_dir.hasWire())
		const_cast<Interest*>(this)->setHopDir(6666);
	if(m_hop_dir.value_size() == sizeof(uint32_t))
		dir = *reinterpret_cast<const uint32_t*>(m_hop_dir.value());
	else
	{
		dir = readNonNegativeInteger(m_hop_dir);
	}

	if(dir)
		d = dir/100000.0;
	else
		d = 0.0;
	return d;
}

// ID/////////////////////////////////
Interest&
Interest::setHopId(uint32_t p_id)
{
	if(m_hop_id.hasWire() && m_hop_id.value_size() == sizeof(uint32_t))
	{
		std::memcpy(const_cast<uint8_t*>(m_hop_id.value()),&p_id,sizeof(p_id));
	}
	else
	{
		m_hop_id = makeBinaryBlock(tlv::LastHop,
				reinterpret_cast<const uint8_t*>(&p_id),
				sizeof(p_id));
		m_wire.reset();
	}
	return *this;
}

uint32_t
Interest::getHopId() const
{
	uint32_t hopId;
	if(!m_hop_id.hasWire())
		const_cast<Interest*>(this)->setHopId(random::generateWord32());
	if(m_hop_id.value_size() == sizeof(uint32_t))
		hopId = *reinterpret_cast<const uint32_t*>(m_hop_id.value());
	else
	{
		hopId = readNonNegativeInteger(m_hop_id);
	}

	return hopId;
}


// ---- operators ----

std::ostream&
operator<<(std::ostream& os, const Interest& interest)
{
  os << interest.getName();

  char delim = '?';

  if (interest.getMinSuffixComponents() >= 0) {
    os << delim << "ndn.MinSuffixComponents=" << interest.getMinSuffixComponents();
    delim = '&';
  }
  if (interest.getMaxSuffixComponents() >= 0) {
    os << delim << "ndn.MaxSuffixComponents=" << interest.getMaxSuffixComponents();
    delim = '&';
  }
  if (interest.getChildSelector() != DEFAULT_CHILD_SELECTOR) {
    os << delim << "ndn.ChildSelector=" << interest.getChildSelector();
    delim = '&';
  }
  if (interest.getMustBeFresh()) {
    os << delim << "ndn.MustBeFresh=" << interest.getMustBeFresh();
    delim = '&';
  }
  if (interest.getInterestLifetime() != DEFAULT_INTEREST_LIFETIME) {
    os << delim << "ndn.InterestLifetime=" << interest.getInterestLifetime().count();
    delim = '&';
  }

  if (interest.hasNonce()) {
    os << delim << "ndn.Nonce=" << interest.getNonce();
    delim = '&';
  }
  if (!interest.getExclude().empty()) {
    os << delim << "ndn.Exclude=" << interest.getExclude();
    delim = '&';
  }

  return os;
}

} // namespace ndn
