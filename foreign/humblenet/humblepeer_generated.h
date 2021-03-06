// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_HUMBLEPEER_HUMBLENET_HUMBLEPEER_H_
#define FLATBUFFERS_GENERATED_HUMBLEPEER_HUMBLENET_HUMBLEPEER_H_

#include "flatbuffers/flatbuffers.h"

namespace humblenet {
namespace HumblePeer {

struct Attribute;

struct ICEServer;

struct HelloServer;

struct HelloClient;

struct P2POffer;

struct P2PAnswer;

struct P2PConnected;

struct P2PDisconnect;

struct P2PReject;

struct ICECandidate;

struct P2PRelayData;

struct AliasRegister;

struct AliasUnregister;

struct AliasLookup;

struct AliasResolved;

struct Message;

enum class ICEServerType : uint8_t {
  STUNServer = 1,
  TURNServer = 2,
  MIN = STUNServer,
  MAX = TURNServer
};

inline const char **EnumNamesICEServerType() {
  static const char *names[] = { "STUNServer", "TURNServer", nullptr };
  return names;
}

inline const char *EnumNameICEServerType(ICEServerType e) { return EnumNamesICEServerType()[static_cast<int>(e) - static_cast<int>(ICEServerType::STUNServer)]; }

enum class P2PRejectReason : uint8_t {
  NotFound = 1,
  PeerRefused = 2,
  MIN = NotFound,
  MAX = PeerRefused
};

inline const char **EnumNamesP2PRejectReason() {
  static const char *names[] = { "NotFound", "PeerRefused", nullptr };
  return names;
}

inline const char *EnumNameP2PRejectReason(P2PRejectReason e) { return EnumNamesP2PRejectReason()[static_cast<int>(e) - static_cast<int>(P2PRejectReason::NotFound)]; }

enum class MessageType : uint8_t {
  NONE = 0,
  HelloServer = 1,
  HelloClient = 2,
  P2PConnected = 10,
  P2PDisconnect = 11,
  P2POffer = 12,
  P2PAnswer = 13,
  P2PReject = 14,
  ICECandidate = 15,
  P2PRelayData = 16,
  AliasRegister = 20,
  AliasUnregister = 21,
  AliasLookup = 22,
  AliasResolved = 23,
  MIN = NONE,
  MAX = AliasResolved
};

inline const char **EnumNamesMessageType() {
  static const char *names[] = { "NONE", "HelloServer", "HelloClient", "", "", "", "", "", "", "", "P2PConnected", "P2PDisconnect", "P2POffer", "P2PAnswer", "P2PReject", "ICECandidate", "P2PRelayData", "", "", "", "AliasRegister", "AliasUnregister", "AliasLookup", "AliasResolved", nullptr };
  return names;
}

inline const char *EnumNameMessageType(MessageType e) { return EnumNamesMessageType()[static_cast<int>(e)]; }

inline bool VerifyMessageType(flatbuffers::Verifier &verifier, const void *union_obj, MessageType type);

struct Attribute FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_KEY = 4,
    VT_VALUE = 6
  };
  const flatbuffers::String *key() const { return GetPointer<const flatbuffers::String *>(VT_KEY); }
  bool KeyCompareLessThan(const Attribute *o) const { return *key() < *o->key(); }
  int KeyCompareWithValue(const char *val) const { return strcmp(key()->c_str(), val); }
  const flatbuffers::String *value() const { return GetPointer<const flatbuffers::String *>(VT_VALUE); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyFieldRequired<flatbuffers::uoffset_t>(verifier, VT_KEY) &&
           verifier.Verify(key()) &&
           VerifyFieldRequired<flatbuffers::uoffset_t>(verifier, VT_VALUE) &&
           verifier.Verify(value()) &&
           verifier.EndTable();
  }
};

struct AttributeBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_key(flatbuffers::Offset<flatbuffers::String> key) { fbb_.AddOffset(Attribute::VT_KEY, key); }
  void add_value(flatbuffers::Offset<flatbuffers::String> value) { fbb_.AddOffset(Attribute::VT_VALUE, value); }
  AttributeBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  AttributeBuilder &operator=(const AttributeBuilder &);
  flatbuffers::Offset<Attribute> Finish() {
    auto o = flatbuffers::Offset<Attribute>(fbb_.EndTable(start_, 2));
    fbb_.Required(o, Attribute::VT_KEY);  // key
    fbb_.Required(o, Attribute::VT_VALUE);  // value
    return o;
  }
};

inline flatbuffers::Offset<Attribute> CreateAttribute(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<flatbuffers::String> key = 0,
   flatbuffers::Offset<flatbuffers::String> value = 0) {
  AttributeBuilder builder_(_fbb);
  builder_.add_value(value);
  builder_.add_key(key);
  return builder_.Finish();
}

struct ICEServer FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_TYPE = 4,
    VT_SERVER = 6,
    VT_USERNAME = 8,
    VT_PASSWORD = 10
  };
  ICEServerType type() const { return static_cast<ICEServerType>(GetField<uint8_t>(VT_TYPE, 1)); }
  const flatbuffers::String *server() const { return GetPointer<const flatbuffers::String *>(VT_SERVER); }
  const flatbuffers::String *username() const { return GetPointer<const flatbuffers::String *>(VT_USERNAME); }
  const flatbuffers::String *password() const { return GetPointer<const flatbuffers::String *>(VT_PASSWORD); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_TYPE) &&
           VerifyFieldRequired<flatbuffers::uoffset_t>(verifier, VT_SERVER) &&
           verifier.Verify(server()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_USERNAME) &&
           verifier.Verify(username()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_PASSWORD) &&
           verifier.Verify(password()) &&
           verifier.EndTable();
  }
};

struct ICEServerBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_type(ICEServerType type) { fbb_.AddElement<uint8_t>(ICEServer::VT_TYPE, static_cast<uint8_t>(type), 1); }
  void add_server(flatbuffers::Offset<flatbuffers::String> server) { fbb_.AddOffset(ICEServer::VT_SERVER, server); }
  void add_username(flatbuffers::Offset<flatbuffers::String> username) { fbb_.AddOffset(ICEServer::VT_USERNAME, username); }
  void add_password(flatbuffers::Offset<flatbuffers::String> password) { fbb_.AddOffset(ICEServer::VT_PASSWORD, password); }
  ICEServerBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  ICEServerBuilder &operator=(const ICEServerBuilder &);
  flatbuffers::Offset<ICEServer> Finish() {
    auto o = flatbuffers::Offset<ICEServer>(fbb_.EndTable(start_, 4));
    fbb_.Required(o, ICEServer::VT_SERVER);  // server
    return o;
  }
};

inline flatbuffers::Offset<ICEServer> CreateICEServer(flatbuffers::FlatBufferBuilder &_fbb,
   ICEServerType type = ICEServerType::STUNServer,
   flatbuffers::Offset<flatbuffers::String> server = 0,
   flatbuffers::Offset<flatbuffers::String> username = 0,
   flatbuffers::Offset<flatbuffers::String> password = 0) {
  ICEServerBuilder builder_(_fbb);
  builder_.add_password(password);
  builder_.add_username(username);
  builder_.add_server(server);
  builder_.add_type(type);
  return builder_.Finish();
}

struct HelloServer FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_VERSION = 4,
    VT_FLAGS = 6,
    VT_GAMETOKEN = 8,
    VT_GAMESIGNATURE = 10,
    VT_AUTHTOKEN = 12,
    VT_RECONNECTTOKEN = 14,
    VT_ATTRIBUTES = 16
  };
  uint32_t version() const { return GetField<uint32_t>(VT_VERSION, 0); }
  uint8_t flags() const { return GetField<uint8_t>(VT_FLAGS, 0); }
  const flatbuffers::String *gameToken() const { return GetPointer<const flatbuffers::String *>(VT_GAMETOKEN); }
  const flatbuffers::String *gameSignature() const { return GetPointer<const flatbuffers::String *>(VT_GAMESIGNATURE); }
  const flatbuffers::String *authToken() const { return GetPointer<const flatbuffers::String *>(VT_AUTHTOKEN); }
  const flatbuffers::String *reconnectToken() const { return GetPointer<const flatbuffers::String *>(VT_RECONNECTTOKEN); }
  const flatbuffers::Vector<flatbuffers::Offset<Attribute>> *attributes() const { return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<Attribute>> *>(VT_ATTRIBUTES); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_VERSION) &&
           VerifyField<uint8_t>(verifier, VT_FLAGS) &&
           VerifyFieldRequired<flatbuffers::uoffset_t>(verifier, VT_GAMETOKEN) &&
           verifier.Verify(gameToken()) &&
           VerifyFieldRequired<flatbuffers::uoffset_t>(verifier, VT_GAMESIGNATURE) &&
           verifier.Verify(gameSignature()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_AUTHTOKEN) &&
           verifier.Verify(authToken()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_RECONNECTTOKEN) &&
           verifier.Verify(reconnectToken()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_ATTRIBUTES) &&
           verifier.Verify(attributes()) &&
           verifier.VerifyVectorOfTables(attributes()) &&
           verifier.EndTable();
  }
};

struct HelloServerBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_version(uint32_t version) { fbb_.AddElement<uint32_t>(HelloServer::VT_VERSION, version, 0); }
  void add_flags(uint8_t flags) { fbb_.AddElement<uint8_t>(HelloServer::VT_FLAGS, flags, 0); }
  void add_gameToken(flatbuffers::Offset<flatbuffers::String> gameToken) { fbb_.AddOffset(HelloServer::VT_GAMETOKEN, gameToken); }
  void add_gameSignature(flatbuffers::Offset<flatbuffers::String> gameSignature) { fbb_.AddOffset(HelloServer::VT_GAMESIGNATURE, gameSignature); }
  void add_authToken(flatbuffers::Offset<flatbuffers::String> authToken) { fbb_.AddOffset(HelloServer::VT_AUTHTOKEN, authToken); }
  void add_reconnectToken(flatbuffers::Offset<flatbuffers::String> reconnectToken) { fbb_.AddOffset(HelloServer::VT_RECONNECTTOKEN, reconnectToken); }
  void add_attributes(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Attribute>>> attributes) { fbb_.AddOffset(HelloServer::VT_ATTRIBUTES, attributes); }
  HelloServerBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  HelloServerBuilder &operator=(const HelloServerBuilder &);
  flatbuffers::Offset<HelloServer> Finish() {
    auto o = flatbuffers::Offset<HelloServer>(fbb_.EndTable(start_, 7));
    fbb_.Required(o, HelloServer::VT_GAMETOKEN);  // gameToken
    fbb_.Required(o, HelloServer::VT_GAMESIGNATURE);  // gameSignature
    return o;
  }
};

inline flatbuffers::Offset<HelloServer> CreateHelloServer(flatbuffers::FlatBufferBuilder &_fbb,
   uint32_t version = 0,
   uint8_t flags = 0,
   flatbuffers::Offset<flatbuffers::String> gameToken = 0,
   flatbuffers::Offset<flatbuffers::String> gameSignature = 0,
   flatbuffers::Offset<flatbuffers::String> authToken = 0,
   flatbuffers::Offset<flatbuffers::String> reconnectToken = 0,
   flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Attribute>>> attributes = 0) {
  HelloServerBuilder builder_(_fbb);
  builder_.add_attributes(attributes);
  builder_.add_reconnectToken(reconnectToken);
  builder_.add_authToken(authToken);
  builder_.add_gameSignature(gameSignature);
  builder_.add_gameToken(gameToken);
  builder_.add_version(version);
  builder_.add_flags(flags);
  return builder_.Finish();
}

struct HelloClient FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_PEERID = 4,
    VT_RECONNECTTOKEN = 6,
    VT_ICESERVERS = 8
  };
  uint32_t peerId() const { return GetField<uint32_t>(VT_PEERID, 0); }
  const flatbuffers::String *reconnectToken() const { return GetPointer<const flatbuffers::String *>(VT_RECONNECTTOKEN); }
  const flatbuffers::Vector<flatbuffers::Offset<ICEServer>> *iceServers() const { return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<ICEServer>> *>(VT_ICESERVERS); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_PEERID) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_RECONNECTTOKEN) &&
           verifier.Verify(reconnectToken()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_ICESERVERS) &&
           verifier.Verify(iceServers()) &&
           verifier.VerifyVectorOfTables(iceServers()) &&
           verifier.EndTable();
  }
};

struct HelloClientBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_peerId(uint32_t peerId) { fbb_.AddElement<uint32_t>(HelloClient::VT_PEERID, peerId, 0); }
  void add_reconnectToken(flatbuffers::Offset<flatbuffers::String> reconnectToken) { fbb_.AddOffset(HelloClient::VT_RECONNECTTOKEN, reconnectToken); }
  void add_iceServers(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<ICEServer>>> iceServers) { fbb_.AddOffset(HelloClient::VT_ICESERVERS, iceServers); }
  HelloClientBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  HelloClientBuilder &operator=(const HelloClientBuilder &);
  flatbuffers::Offset<HelloClient> Finish() {
    auto o = flatbuffers::Offset<HelloClient>(fbb_.EndTable(start_, 3));
    return o;
  }
};

inline flatbuffers::Offset<HelloClient> CreateHelloClient(flatbuffers::FlatBufferBuilder &_fbb,
   uint32_t peerId = 0,
   flatbuffers::Offset<flatbuffers::String> reconnectToken = 0,
   flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<ICEServer>>> iceServers = 0) {
  HelloClientBuilder builder_(_fbb);
  builder_.add_iceServers(iceServers);
  builder_.add_reconnectToken(reconnectToken);
  builder_.add_peerId(peerId);
  return builder_.Finish();
}

struct P2POffer FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_PEERID = 4,
    VT_FLAGS = 6,
    VT_OFFER = 8
  };
  uint32_t peerId() const { return GetField<uint32_t>(VT_PEERID, 0); }
  uint8_t flags() const { return GetField<uint8_t>(VT_FLAGS, 0); }
  const flatbuffers::String *offer() const { return GetPointer<const flatbuffers::String *>(VT_OFFER); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_PEERID) &&
           VerifyField<uint8_t>(verifier, VT_FLAGS) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_OFFER) &&
           verifier.Verify(offer()) &&
           verifier.EndTable();
  }
};

struct P2POfferBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_peerId(uint32_t peerId) { fbb_.AddElement<uint32_t>(P2POffer::VT_PEERID, peerId, 0); }
  void add_flags(uint8_t flags) { fbb_.AddElement<uint8_t>(P2POffer::VT_FLAGS, flags, 0); }
  void add_offer(flatbuffers::Offset<flatbuffers::String> offer) { fbb_.AddOffset(P2POffer::VT_OFFER, offer); }
  P2POfferBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  P2POfferBuilder &operator=(const P2POfferBuilder &);
  flatbuffers::Offset<P2POffer> Finish() {
    auto o = flatbuffers::Offset<P2POffer>(fbb_.EndTable(start_, 3));
    return o;
  }
};

inline flatbuffers::Offset<P2POffer> CreateP2POffer(flatbuffers::FlatBufferBuilder &_fbb,
   uint32_t peerId = 0,
   uint8_t flags = 0,
   flatbuffers::Offset<flatbuffers::String> offer = 0) {
  P2POfferBuilder builder_(_fbb);
  builder_.add_offer(offer);
  builder_.add_peerId(peerId);
  builder_.add_flags(flags);
  return builder_.Finish();
}

struct P2PAnswer FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_PEERID = 4,
    VT_OFFER = 6
  };
  uint32_t peerId() const { return GetField<uint32_t>(VT_PEERID, 0); }
  const flatbuffers::String *offer() const { return GetPointer<const flatbuffers::String *>(VT_OFFER); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_PEERID) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_OFFER) &&
           verifier.Verify(offer()) &&
           verifier.EndTable();
  }
};

struct P2PAnswerBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_peerId(uint32_t peerId) { fbb_.AddElement<uint32_t>(P2PAnswer::VT_PEERID, peerId, 0); }
  void add_offer(flatbuffers::Offset<flatbuffers::String> offer) { fbb_.AddOffset(P2PAnswer::VT_OFFER, offer); }
  P2PAnswerBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  P2PAnswerBuilder &operator=(const P2PAnswerBuilder &);
  flatbuffers::Offset<P2PAnswer> Finish() {
    auto o = flatbuffers::Offset<P2PAnswer>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<P2PAnswer> CreateP2PAnswer(flatbuffers::FlatBufferBuilder &_fbb,
   uint32_t peerId = 0,
   flatbuffers::Offset<flatbuffers::String> offer = 0) {
  P2PAnswerBuilder builder_(_fbb);
  builder_.add_offer(offer);
  builder_.add_peerId(peerId);
  return builder_.Finish();
}

struct P2PConnected FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_PEERID = 4
  };
  uint32_t peerId() const { return GetField<uint32_t>(VT_PEERID, 0); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_PEERID) &&
           verifier.EndTable();
  }
};

struct P2PConnectedBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_peerId(uint32_t peerId) { fbb_.AddElement<uint32_t>(P2PConnected::VT_PEERID, peerId, 0); }
  P2PConnectedBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  P2PConnectedBuilder &operator=(const P2PConnectedBuilder &);
  flatbuffers::Offset<P2PConnected> Finish() {
    auto o = flatbuffers::Offset<P2PConnected>(fbb_.EndTable(start_, 1));
    return o;
  }
};

inline flatbuffers::Offset<P2PConnected> CreateP2PConnected(flatbuffers::FlatBufferBuilder &_fbb,
   uint32_t peerId = 0) {
  P2PConnectedBuilder builder_(_fbb);
  builder_.add_peerId(peerId);
  return builder_.Finish();
}

struct P2PDisconnect FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_PEERID = 4
  };
  uint32_t peerId() const { return GetField<uint32_t>(VT_PEERID, 0); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_PEERID) &&
           verifier.EndTable();
  }
};

struct P2PDisconnectBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_peerId(uint32_t peerId) { fbb_.AddElement<uint32_t>(P2PDisconnect::VT_PEERID, peerId, 0); }
  P2PDisconnectBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  P2PDisconnectBuilder &operator=(const P2PDisconnectBuilder &);
  flatbuffers::Offset<P2PDisconnect> Finish() {
    auto o = flatbuffers::Offset<P2PDisconnect>(fbb_.EndTable(start_, 1));
    return o;
  }
};

inline flatbuffers::Offset<P2PDisconnect> CreateP2PDisconnect(flatbuffers::FlatBufferBuilder &_fbb,
   uint32_t peerId = 0) {
  P2PDisconnectBuilder builder_(_fbb);
  builder_.add_peerId(peerId);
  return builder_.Finish();
}

struct P2PReject FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_PEERID = 4,
    VT_REASON = 6
  };
  uint32_t peerId() const { return GetField<uint32_t>(VT_PEERID, 0); }
  P2PRejectReason reason() const { return static_cast<P2PRejectReason>(GetField<uint8_t>(VT_REASON, 1)); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_PEERID) &&
           VerifyField<uint8_t>(verifier, VT_REASON) &&
           verifier.EndTable();
  }
};

struct P2PRejectBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_peerId(uint32_t peerId) { fbb_.AddElement<uint32_t>(P2PReject::VT_PEERID, peerId, 0); }
  void add_reason(P2PRejectReason reason) { fbb_.AddElement<uint8_t>(P2PReject::VT_REASON, static_cast<uint8_t>(reason), 1); }
  P2PRejectBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  P2PRejectBuilder &operator=(const P2PRejectBuilder &);
  flatbuffers::Offset<P2PReject> Finish() {
    auto o = flatbuffers::Offset<P2PReject>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<P2PReject> CreateP2PReject(flatbuffers::FlatBufferBuilder &_fbb,
   uint32_t peerId = 0,
   P2PRejectReason reason = P2PRejectReason::NotFound) {
  P2PRejectBuilder builder_(_fbb);
  builder_.add_peerId(peerId);
  builder_.add_reason(reason);
  return builder_.Finish();
}

struct ICECandidate FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_PEERID = 4,
    VT_OFFER = 6
  };
  uint32_t peerId() const { return GetField<uint32_t>(VT_PEERID, 0); }
  const flatbuffers::String *offer() const { return GetPointer<const flatbuffers::String *>(VT_OFFER); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_PEERID) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_OFFER) &&
           verifier.Verify(offer()) &&
           verifier.EndTable();
  }
};

struct ICECandidateBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_peerId(uint32_t peerId) { fbb_.AddElement<uint32_t>(ICECandidate::VT_PEERID, peerId, 0); }
  void add_offer(flatbuffers::Offset<flatbuffers::String> offer) { fbb_.AddOffset(ICECandidate::VT_OFFER, offer); }
  ICECandidateBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  ICECandidateBuilder &operator=(const ICECandidateBuilder &);
  flatbuffers::Offset<ICECandidate> Finish() {
    auto o = flatbuffers::Offset<ICECandidate>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<ICECandidate> CreateICECandidate(flatbuffers::FlatBufferBuilder &_fbb,
   uint32_t peerId = 0,
   flatbuffers::Offset<flatbuffers::String> offer = 0) {
  ICECandidateBuilder builder_(_fbb);
  builder_.add_offer(offer);
  builder_.add_peerId(peerId);
  return builder_.Finish();
}

struct P2PRelayData FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_PEERID = 4,
    VT_DATA = 6
  };
  uint32_t peerId() const { return GetField<uint32_t>(VT_PEERID, 0); }
  const flatbuffers::Vector<int8_t> *data() const { return GetPointer<const flatbuffers::Vector<int8_t> *>(VT_DATA); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_PEERID) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_DATA) &&
           verifier.Verify(data()) &&
           verifier.EndTable();
  }
};

struct P2PRelayDataBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_peerId(uint32_t peerId) { fbb_.AddElement<uint32_t>(P2PRelayData::VT_PEERID, peerId, 0); }
  void add_data(flatbuffers::Offset<flatbuffers::Vector<int8_t>> data) { fbb_.AddOffset(P2PRelayData::VT_DATA, data); }
  P2PRelayDataBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  P2PRelayDataBuilder &operator=(const P2PRelayDataBuilder &);
  flatbuffers::Offset<P2PRelayData> Finish() {
    auto o = flatbuffers::Offset<P2PRelayData>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<P2PRelayData> CreateP2PRelayData(flatbuffers::FlatBufferBuilder &_fbb,
   uint32_t peerId = 0,
   flatbuffers::Offset<flatbuffers::Vector<int8_t>> data = 0) {
  P2PRelayDataBuilder builder_(_fbb);
  builder_.add_data(data);
  builder_.add_peerId(peerId);
  return builder_.Finish();
}

struct AliasRegister FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_ALIAS = 4
  };
  const flatbuffers::String *alias() const { return GetPointer<const flatbuffers::String *>(VT_ALIAS); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyFieldRequired<flatbuffers::uoffset_t>(verifier, VT_ALIAS) &&
           verifier.Verify(alias()) &&
           verifier.EndTable();
  }
};

struct AliasRegisterBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_alias(flatbuffers::Offset<flatbuffers::String> alias) { fbb_.AddOffset(AliasRegister::VT_ALIAS, alias); }
  AliasRegisterBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  AliasRegisterBuilder &operator=(const AliasRegisterBuilder &);
  flatbuffers::Offset<AliasRegister> Finish() {
    auto o = flatbuffers::Offset<AliasRegister>(fbb_.EndTable(start_, 1));
    fbb_.Required(o, AliasRegister::VT_ALIAS);  // alias
    return o;
  }
};

inline flatbuffers::Offset<AliasRegister> CreateAliasRegister(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<flatbuffers::String> alias = 0) {
  AliasRegisterBuilder builder_(_fbb);
  builder_.add_alias(alias);
  return builder_.Finish();
}

struct AliasUnregister FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_ALIAS = 4
  };
  const flatbuffers::String *alias() const { return GetPointer<const flatbuffers::String *>(VT_ALIAS); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_ALIAS) &&
           verifier.Verify(alias()) &&
           verifier.EndTable();
  }
};

struct AliasUnregisterBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_alias(flatbuffers::Offset<flatbuffers::String> alias) { fbb_.AddOffset(AliasUnregister::VT_ALIAS, alias); }
  AliasUnregisterBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  AliasUnregisterBuilder &operator=(const AliasUnregisterBuilder &);
  flatbuffers::Offset<AliasUnregister> Finish() {
    auto o = flatbuffers::Offset<AliasUnregister>(fbb_.EndTable(start_, 1));
    return o;
  }
};

inline flatbuffers::Offset<AliasUnregister> CreateAliasUnregister(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<flatbuffers::String> alias = 0) {
  AliasUnregisterBuilder builder_(_fbb);
  builder_.add_alias(alias);
  return builder_.Finish();
}

struct AliasLookup FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_ALIAS = 4
  };
  const flatbuffers::String *alias() const { return GetPointer<const flatbuffers::String *>(VT_ALIAS); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyFieldRequired<flatbuffers::uoffset_t>(verifier, VT_ALIAS) &&
           verifier.Verify(alias()) &&
           verifier.EndTable();
  }
};

struct AliasLookupBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_alias(flatbuffers::Offset<flatbuffers::String> alias) { fbb_.AddOffset(AliasLookup::VT_ALIAS, alias); }
  AliasLookupBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  AliasLookupBuilder &operator=(const AliasLookupBuilder &);
  flatbuffers::Offset<AliasLookup> Finish() {
    auto o = flatbuffers::Offset<AliasLookup>(fbb_.EndTable(start_, 1));
    fbb_.Required(o, AliasLookup::VT_ALIAS);  // alias
    return o;
  }
};

inline flatbuffers::Offset<AliasLookup> CreateAliasLookup(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<flatbuffers::String> alias = 0) {
  AliasLookupBuilder builder_(_fbb);
  builder_.add_alias(alias);
  return builder_.Finish();
}

struct AliasResolved FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_ALIAS = 4,
    VT_PEERID = 6
  };
  const flatbuffers::String *alias() const { return GetPointer<const flatbuffers::String *>(VT_ALIAS); }
  uint32_t peerId() const { return GetField<uint32_t>(VT_PEERID, 0); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyFieldRequired<flatbuffers::uoffset_t>(verifier, VT_ALIAS) &&
           verifier.Verify(alias()) &&
           VerifyField<uint32_t>(verifier, VT_PEERID) &&
           verifier.EndTable();
  }
};

struct AliasResolvedBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_alias(flatbuffers::Offset<flatbuffers::String> alias) { fbb_.AddOffset(AliasResolved::VT_ALIAS, alias); }
  void add_peerId(uint32_t peerId) { fbb_.AddElement<uint32_t>(AliasResolved::VT_PEERID, peerId, 0); }
  AliasResolvedBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  AliasResolvedBuilder &operator=(const AliasResolvedBuilder &);
  flatbuffers::Offset<AliasResolved> Finish() {
    auto o = flatbuffers::Offset<AliasResolved>(fbb_.EndTable(start_, 2));
    fbb_.Required(o, AliasResolved::VT_ALIAS);  // alias
    return o;
  }
};

inline flatbuffers::Offset<AliasResolved> CreateAliasResolved(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<flatbuffers::String> alias = 0,
   uint32_t peerId = 0) {
  AliasResolvedBuilder builder_(_fbb);
  builder_.add_peerId(peerId);
  builder_.add_alias(alias);
  return builder_.Finish();
}

struct Message FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_MESSAGE_TYPE = 4,
    VT_MESSAGE = 6
  };
  MessageType message_type() const { return static_cast<MessageType>(GetField<uint8_t>(VT_MESSAGE_TYPE, 0)); }
  const void *message() const { return GetPointer<const void *>(VT_MESSAGE); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_MESSAGE_TYPE) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_MESSAGE) &&
           VerifyMessageType(verifier, message(), message_type()) &&
           verifier.EndTable();
  }
};

struct MessageBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_message_type(MessageType message_type) { fbb_.AddElement<uint8_t>(Message::VT_MESSAGE_TYPE, static_cast<uint8_t>(message_type), 0); }
  void add_message(flatbuffers::Offset<void> message) { fbb_.AddOffset(Message::VT_MESSAGE, message); }
  MessageBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  MessageBuilder &operator=(const MessageBuilder &);
  flatbuffers::Offset<Message> Finish() {
    auto o = flatbuffers::Offset<Message>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<Message> CreateMessage(flatbuffers::FlatBufferBuilder &_fbb,
   MessageType message_type = MessageType::NONE,
   flatbuffers::Offset<void> message = 0) {
  MessageBuilder builder_(_fbb);
  builder_.add_message(message);
  builder_.add_message_type(message_type);
  return builder_.Finish();
}

inline bool VerifyMessageType(flatbuffers::Verifier &verifier, const void *union_obj, MessageType type) {
  switch (type) {
    case MessageType::NONE: return true;
    case MessageType::HelloServer: return verifier.VerifyTable(reinterpret_cast<const HelloServer *>(union_obj));
    case MessageType::HelloClient: return verifier.VerifyTable(reinterpret_cast<const HelloClient *>(union_obj));
    case MessageType::P2PConnected: return verifier.VerifyTable(reinterpret_cast<const P2PConnected *>(union_obj));
    case MessageType::P2PDisconnect: return verifier.VerifyTable(reinterpret_cast<const P2PDisconnect *>(union_obj));
    case MessageType::P2POffer: return verifier.VerifyTable(reinterpret_cast<const P2POffer *>(union_obj));
    case MessageType::P2PAnswer: return verifier.VerifyTable(reinterpret_cast<const P2PAnswer *>(union_obj));
    case MessageType::P2PReject: return verifier.VerifyTable(reinterpret_cast<const P2PReject *>(union_obj));
    case MessageType::ICECandidate: return verifier.VerifyTable(reinterpret_cast<const ICECandidate *>(union_obj));
    case MessageType::P2PRelayData: return verifier.VerifyTable(reinterpret_cast<const P2PRelayData *>(union_obj));
    case MessageType::AliasRegister: return verifier.VerifyTable(reinterpret_cast<const AliasRegister *>(union_obj));
    case MessageType::AliasUnregister: return verifier.VerifyTable(reinterpret_cast<const AliasUnregister *>(union_obj));
    case MessageType::AliasLookup: return verifier.VerifyTable(reinterpret_cast<const AliasLookup *>(union_obj));
    case MessageType::AliasResolved: return verifier.VerifyTable(reinterpret_cast<const AliasResolved *>(union_obj));
    default: return false;
  }
}

inline const humblenet::HumblePeer::Message *GetMessage(const void *buf) { return flatbuffers::GetRoot<humblenet::HumblePeer::Message>(buf); }

inline bool VerifyMessageBuffer(flatbuffers::Verifier &verifier) { return verifier.VerifyBuffer<humblenet::HumblePeer::Message>(); }

inline void FinishMessageBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<humblenet::HumblePeer::Message> root) { fbb.Finish(root); }

}  // namespace HumblePeer
}  // namespace humblenet

#endif  // FLATBUFFERS_GENERATED_HUMBLEPEER_HUMBLENET_HUMBLEPEER_H_
