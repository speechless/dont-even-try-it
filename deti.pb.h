// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: deti.proto

#ifndef PROTOBUF_deti_2eproto__INCLUDED
#define PROTOBUF_deti_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace DETI {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_deti_2eproto();
void protobuf_AssignDesc_deti_2eproto();
void protobuf_ShutdownFile_deti_2eproto();

class Account;
class Database;

// ===================================================================

class Account : public ::google::protobuf::Message {
 public:
  Account();
  virtual ~Account();

  Account(const Account& from);

  inline Account& operator=(const Account& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Account& default_instance();

  void Swap(Account* other);

  // implements Message ----------------------------------------------

  Account* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Account& from);
  void MergeFrom(const Account& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional string username = 1;
  inline bool has_username() const;
  inline void clear_username();
  static const int kUsernameFieldNumber = 1;
  inline const ::std::string& username() const;
  inline void set_username(const ::std::string& value);
  inline void set_username(const char* value);
  inline void set_username(const char* value, size_t size);
  inline ::std::string* mutable_username();
  inline ::std::string* release_username();
  inline void set_allocated_username(::std::string* username);

  // optional string password = 2;
  inline bool has_password() const;
  inline void clear_password();
  static const int kPasswordFieldNumber = 2;
  inline const ::std::string& password() const;
  inline void set_password(const ::std::string& value);
  inline void set_password(const char* value);
  inline void set_password(const char* value, size_t size);
  inline ::std::string* mutable_password();
  inline ::std::string* release_password();
  inline void set_allocated_password(::std::string* password);

  // optional string email = 3;
  inline bool has_email() const;
  inline void clear_email();
  static const int kEmailFieldNumber = 3;
  inline const ::std::string& email() const;
  inline void set_email(const ::std::string& value);
  inline void set_email(const char* value);
  inline void set_email(const char* value, size_t size);
  inline ::std::string* mutable_email();
  inline ::std::string* release_email();
  inline void set_allocated_email(::std::string* email);

  // optional bool activated = 4;
  inline bool has_activated() const;
  inline void clear_activated();
  static const int kActivatedFieldNumber = 4;
  inline bool activated() const;
  inline void set_activated(bool value);

  // @@protoc_insertion_point(class_scope:DETI.Account)
 private:
  inline void set_has_username();
  inline void clear_has_username();
  inline void set_has_password();
  inline void clear_has_password();
  inline void set_has_email();
  inline void clear_has_email();
  inline void set_has_activated();
  inline void clear_has_activated();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* username_;
  ::std::string* password_;
  ::std::string* email_;
  bool activated_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];

  friend void  protobuf_AddDesc_deti_2eproto();
  friend void protobuf_AssignDesc_deti_2eproto();
  friend void protobuf_ShutdownFile_deti_2eproto();

  void InitAsDefaultInstance();
  static Account* default_instance_;
};
// -------------------------------------------------------------------

class Database : public ::google::protobuf::Message {
 public:
  Database();
  virtual ~Database();

  Database(const Database& from);

  inline Database& operator=(const Database& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Database& default_instance();

  void Swap(Database* other);

  // implements Message ----------------------------------------------

  Database* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Database& from);
  void MergeFrom(const Database& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .DETI.Account account = 1;
  inline int account_size() const;
  inline void clear_account();
  static const int kAccountFieldNumber = 1;
  inline const ::DETI::Account& account(int index) const;
  inline ::DETI::Account* mutable_account(int index);
  inline ::DETI::Account* add_account();
  inline const ::google::protobuf::RepeatedPtrField< ::DETI::Account >&
      account() const;
  inline ::google::protobuf::RepeatedPtrField< ::DETI::Account >*
      mutable_account();

  // @@protoc_insertion_point(class_scope:DETI.Database)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::DETI::Account > account_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_deti_2eproto();
  friend void protobuf_AssignDesc_deti_2eproto();
  friend void protobuf_ShutdownFile_deti_2eproto();

  void InitAsDefaultInstance();
  static Database* default_instance_;
};
// ===================================================================


// ===================================================================

// Account

// optional string username = 1;
inline bool Account::has_username() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Account::set_has_username() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Account::clear_has_username() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Account::clear_username() {
  if (username_ != &::google::protobuf::internal::kEmptyString) {
    username_->clear();
  }
  clear_has_username();
}
inline const ::std::string& Account::username() const {
  return *username_;
}
inline void Account::set_username(const ::std::string& value) {
  set_has_username();
  if (username_ == &::google::protobuf::internal::kEmptyString) {
    username_ = new ::std::string;
  }
  username_->assign(value);
}
inline void Account::set_username(const char* value) {
  set_has_username();
  if (username_ == &::google::protobuf::internal::kEmptyString) {
    username_ = new ::std::string;
  }
  username_->assign(value);
}
inline void Account::set_username(const char* value, size_t size) {
  set_has_username();
  if (username_ == &::google::protobuf::internal::kEmptyString) {
    username_ = new ::std::string;
  }
  username_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Account::mutable_username() {
  set_has_username();
  if (username_ == &::google::protobuf::internal::kEmptyString) {
    username_ = new ::std::string;
  }
  return username_;
}
inline ::std::string* Account::release_username() {
  clear_has_username();
  if (username_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = username_;
    username_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void Account::set_allocated_username(::std::string* username) {
  if (username_ != &::google::protobuf::internal::kEmptyString) {
    delete username_;
  }
  if (username) {
    set_has_username();
    username_ = username;
  } else {
    clear_has_username();
    username_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional string password = 2;
inline bool Account::has_password() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Account::set_has_password() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Account::clear_has_password() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Account::clear_password() {
  if (password_ != &::google::protobuf::internal::kEmptyString) {
    password_->clear();
  }
  clear_has_password();
}
inline const ::std::string& Account::password() const {
  return *password_;
}
inline void Account::set_password(const ::std::string& value) {
  set_has_password();
  if (password_ == &::google::protobuf::internal::kEmptyString) {
    password_ = new ::std::string;
  }
  password_->assign(value);
}
inline void Account::set_password(const char* value) {
  set_has_password();
  if (password_ == &::google::protobuf::internal::kEmptyString) {
    password_ = new ::std::string;
  }
  password_->assign(value);
}
inline void Account::set_password(const char* value, size_t size) {
  set_has_password();
  if (password_ == &::google::protobuf::internal::kEmptyString) {
    password_ = new ::std::string;
  }
  password_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Account::mutable_password() {
  set_has_password();
  if (password_ == &::google::protobuf::internal::kEmptyString) {
    password_ = new ::std::string;
  }
  return password_;
}
inline ::std::string* Account::release_password() {
  clear_has_password();
  if (password_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = password_;
    password_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void Account::set_allocated_password(::std::string* password) {
  if (password_ != &::google::protobuf::internal::kEmptyString) {
    delete password_;
  }
  if (password) {
    set_has_password();
    password_ = password;
  } else {
    clear_has_password();
    password_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional string email = 3;
inline bool Account::has_email() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Account::set_has_email() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Account::clear_has_email() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Account::clear_email() {
  if (email_ != &::google::protobuf::internal::kEmptyString) {
    email_->clear();
  }
  clear_has_email();
}
inline const ::std::string& Account::email() const {
  return *email_;
}
inline void Account::set_email(const ::std::string& value) {
  set_has_email();
  if (email_ == &::google::protobuf::internal::kEmptyString) {
    email_ = new ::std::string;
  }
  email_->assign(value);
}
inline void Account::set_email(const char* value) {
  set_has_email();
  if (email_ == &::google::protobuf::internal::kEmptyString) {
    email_ = new ::std::string;
  }
  email_->assign(value);
}
inline void Account::set_email(const char* value, size_t size) {
  set_has_email();
  if (email_ == &::google::protobuf::internal::kEmptyString) {
    email_ = new ::std::string;
  }
  email_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Account::mutable_email() {
  set_has_email();
  if (email_ == &::google::protobuf::internal::kEmptyString) {
    email_ = new ::std::string;
  }
  return email_;
}
inline ::std::string* Account::release_email() {
  clear_has_email();
  if (email_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = email_;
    email_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void Account::set_allocated_email(::std::string* email) {
  if (email_ != &::google::protobuf::internal::kEmptyString) {
    delete email_;
  }
  if (email) {
    set_has_email();
    email_ = email;
  } else {
    clear_has_email();
    email_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional bool activated = 4;
inline bool Account::has_activated() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void Account::set_has_activated() {
  _has_bits_[0] |= 0x00000008u;
}
inline void Account::clear_has_activated() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void Account::clear_activated() {
  activated_ = false;
  clear_has_activated();
}
inline bool Account::activated() const {
  return activated_;
}
inline void Account::set_activated(bool value) {
  set_has_activated();
  activated_ = value;
}

// -------------------------------------------------------------------

// Database

// repeated .DETI.Account account = 1;
inline int Database::account_size() const {
  return account_.size();
}
inline void Database::clear_account() {
  account_.Clear();
}
inline const ::DETI::Account& Database::account(int index) const {
  return account_.Get(index);
}
inline ::DETI::Account* Database::mutable_account(int index) {
  return account_.Mutable(index);
}
inline ::DETI::Account* Database::add_account() {
  return account_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::DETI::Account >&
Database::account() const {
  return account_;
}
inline ::google::protobuf::RepeatedPtrField< ::DETI::Account >*
Database::mutable_account() {
  return &account_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace DETI

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_deti_2eproto__INCLUDED
