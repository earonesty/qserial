// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: bench.proto

#ifndef PROTOBUF_bench_2eproto__INCLUDED
#define PROTOBUF_bench_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
// @@protoc_insertion_point(includes)

namespace proto {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_bench_2eproto();
void protobuf_AssignDesc_bench_2eproto();
void protobuf_ShutdownFile_bench_2eproto();

class benchy;

// ===================================================================

class benchy : public ::google::protobuf::MessageLite /* @@protoc_insertion_point(class_definition:proto.benchy) */ {
 public:
  benchy();
  virtual ~benchy();

  benchy(const benchy& from);

  inline benchy& operator=(const benchy& from) {
    CopyFrom(from);
    return *this;
  }

  static const benchy& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const benchy* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(benchy* other);

  // implements Message ----------------------------------------------

  inline benchy* New() const { return New(NULL); }

  benchy* New(::google::protobuf::Arena* arena) const;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from);
  void CopyFrom(const benchy& from);
  void MergeFrom(const benchy& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  void DiscardUnknownFields();
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(benchy* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _arena_ptr_;
  }
  inline ::google::protobuf::Arena* MaybeArenaPtr() const {
    return _arena_ptr_;
  }
  public:

  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional string f0 = 1;
  void clear_f0();
  static const int kF0FieldNumber = 1;
  const ::std::string& f0() const;
  void set_f0(const ::std::string& value);
  void set_f0(const char* value);
  void set_f0(const char* value, size_t size);
  ::std::string* mutable_f0();
  ::std::string* release_f0();
  void set_allocated_f0(::std::string* f0);

  // optional string f1 = 2;
  void clear_f1();
  static const int kF1FieldNumber = 2;
  const ::std::string& f1() const;
  void set_f1(const ::std::string& value);
  void set_f1(const char* value);
  void set_f1(const char* value, size_t size);
  ::std::string* mutable_f1();
  ::std::string* release_f1();
  void set_allocated_f1(::std::string* f1);

  // optional uint64 f2 = 3;
  void clear_f2();
  static const int kF2FieldNumber = 3;
  ::google::protobuf::uint64 f2() const;
  void set_f2(::google::protobuf::uint64 value);

  // optional int64 f3 = 4;
  void clear_f3();
  static const int kF3FieldNumber = 4;
  ::google::protobuf::int64 f3() const;
  void set_f3(::google::protobuf::int64 value);

  // repeated bytes f4 = 5;
  int f4_size() const;
  void clear_f4();
  static const int kF4FieldNumber = 5;
  const ::std::string& f4(int index) const;
  ::std::string* mutable_f4(int index);
  void set_f4(int index, const ::std::string& value);
  void set_f4(int index, const char* value);
  void set_f4(int index, const void* value, size_t size);
  ::std::string* add_f4();
  void add_f4(const ::std::string& value);
  void add_f4(const char* value);
  void add_f4(const void* value, size_t size);
  const ::google::protobuf::RepeatedPtrField< ::std::string>& f4() const;
  ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_f4();

  // @@protoc_insertion_point(class_scope:proto.benchy)
 private:

  ::google::protobuf::internal::ArenaStringPtr _unknown_fields_;
  ::google::protobuf::Arena* _arena_ptr_;

  bool _is_default_instance_;
  ::google::protobuf::internal::ArenaStringPtr f0_;
  ::google::protobuf::internal::ArenaStringPtr f1_;
  ::google::protobuf::uint64 f2_;
  ::google::protobuf::int64 f3_;
  ::google::protobuf::RepeatedPtrField< ::std::string> f4_;
  mutable int _cached_size_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_bench_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_bench_2eproto();
  #endif
  friend void protobuf_AssignDesc_bench_2eproto();
  friend void protobuf_ShutdownFile_bench_2eproto();

  void InitAsDefaultInstance();
  static benchy* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// benchy

// optional string f0 = 1;
inline void benchy::clear_f0() {
  f0_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& benchy::f0() const {
  // @@protoc_insertion_point(field_get:proto.benchy.f0)
  return f0_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void benchy::set_f0(const ::std::string& value) {
  
  f0_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:proto.benchy.f0)
}
inline void benchy::set_f0(const char* value) {
  
  f0_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:proto.benchy.f0)
}
inline void benchy::set_f0(const char* value, size_t size) {
  
  f0_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:proto.benchy.f0)
}
inline ::std::string* benchy::mutable_f0() {
  
  // @@protoc_insertion_point(field_mutable:proto.benchy.f0)
  return f0_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* benchy::release_f0() {
  // @@protoc_insertion_point(field_release:proto.benchy.f0)
  
  return f0_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void benchy::set_allocated_f0(::std::string* f0) {
  if (f0 != NULL) {
    
  } else {
    
  }
  f0_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), f0);
  // @@protoc_insertion_point(field_set_allocated:proto.benchy.f0)
}

// optional string f1 = 2;
inline void benchy::clear_f1() {
  f1_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& benchy::f1() const {
  // @@protoc_insertion_point(field_get:proto.benchy.f1)
  return f1_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void benchy::set_f1(const ::std::string& value) {
  
  f1_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:proto.benchy.f1)
}
inline void benchy::set_f1(const char* value) {
  
  f1_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:proto.benchy.f1)
}
inline void benchy::set_f1(const char* value, size_t size) {
  
  f1_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:proto.benchy.f1)
}
inline ::std::string* benchy::mutable_f1() {
  
  // @@protoc_insertion_point(field_mutable:proto.benchy.f1)
  return f1_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* benchy::release_f1() {
  // @@protoc_insertion_point(field_release:proto.benchy.f1)
  
  return f1_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void benchy::set_allocated_f1(::std::string* f1) {
  if (f1 != NULL) {
    
  } else {
    
  }
  f1_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), f1);
  // @@protoc_insertion_point(field_set_allocated:proto.benchy.f1)
}

// optional uint64 f2 = 3;
inline void benchy::clear_f2() {
  f2_ = GOOGLE_ULONGLONG(0);
}
inline ::google::protobuf::uint64 benchy::f2() const {
  // @@protoc_insertion_point(field_get:proto.benchy.f2)
  return f2_;
}
inline void benchy::set_f2(::google::protobuf::uint64 value) {
  
  f2_ = value;
  // @@protoc_insertion_point(field_set:proto.benchy.f2)
}

// optional int64 f3 = 4;
inline void benchy::clear_f3() {
  f3_ = GOOGLE_LONGLONG(0);
}
inline ::google::protobuf::int64 benchy::f3() const {
  // @@protoc_insertion_point(field_get:proto.benchy.f3)
  return f3_;
}
inline void benchy::set_f3(::google::protobuf::int64 value) {
  
  f3_ = value;
  // @@protoc_insertion_point(field_set:proto.benchy.f3)
}

// repeated bytes f4 = 5;
inline int benchy::f4_size() const {
  return f4_.size();
}
inline void benchy::clear_f4() {
  f4_.Clear();
}
inline const ::std::string& benchy::f4(int index) const {
  // @@protoc_insertion_point(field_get:proto.benchy.f4)
  return f4_.Get(index);
}
inline ::std::string* benchy::mutable_f4(int index) {
  // @@protoc_insertion_point(field_mutable:proto.benchy.f4)
  return f4_.Mutable(index);
}
inline void benchy::set_f4(int index, const ::std::string& value) {
  // @@protoc_insertion_point(field_set:proto.benchy.f4)
  f4_.Mutable(index)->assign(value);
}
inline void benchy::set_f4(int index, const char* value) {
  f4_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:proto.benchy.f4)
}
inline void benchy::set_f4(int index, const void* value, size_t size) {
  f4_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:proto.benchy.f4)
}
inline ::std::string* benchy::add_f4() {
  // @@protoc_insertion_point(field_add_mutable:proto.benchy.f4)
  return f4_.Add();
}
inline void benchy::add_f4(const ::std::string& value) {
  f4_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:proto.benchy.f4)
}
inline void benchy::add_f4(const char* value) {
  f4_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:proto.benchy.f4)
}
inline void benchy::add_f4(const void* value, size_t size) {
  f4_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:proto.benchy.f4)
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
benchy::f4() const {
  // @@protoc_insertion_point(field_list:proto.benchy.f4)
  return f4_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
benchy::mutable_f4() {
  // @@protoc_insertion_point(field_mutable_list:proto.benchy.f4)
  return &f4_;
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace proto

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_bench_2eproto__INCLUDED
