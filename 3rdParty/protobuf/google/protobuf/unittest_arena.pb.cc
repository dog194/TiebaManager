// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/protobuf/unittest_arena.proto

#include "google/protobuf/unittest_arena.pb.h"

#include <algorithm>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/wire_format.h"
#include "google/protobuf/generated_message_tctable_impl.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"
PROTOBUF_PRAGMA_INIT_SEG
namespace _pb = ::google::protobuf;
namespace _pbi = ::google::protobuf::internal;
namespace _fl = ::google::protobuf::internal::field_layout;
namespace proto2_arena_unittest {
        template <typename>
PROTOBUF_CONSTEXPR NestedMessage::NestedMessage(::_pbi::ConstantInitialized)
    : _impl_{
      /*decltype(_impl_._has_bits_)*/ {},
      /*decltype(_impl_._cached_size_)*/ {},
      /*decltype(_impl_.d_)*/ 0,
    } {}
struct NestedMessageDefaultTypeInternal {
  PROTOBUF_CONSTEXPR NestedMessageDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~NestedMessageDefaultTypeInternal() {}
  union {
    NestedMessage _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 NestedMessageDefaultTypeInternal _NestedMessage_default_instance_;
        template <typename>
PROTOBUF_CONSTEXPR ArenaMessage::ArenaMessage(::_pbi::ConstantInitialized)
    : _impl_{
      /*decltype(_impl_.repeated_nested_message_)*/ {},
      /*decltype(_impl_._cached_size_)*/ {},
    } {}
struct ArenaMessageDefaultTypeInternal {
  PROTOBUF_CONSTEXPR ArenaMessageDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~ArenaMessageDefaultTypeInternal() {}
  union {
    ArenaMessage _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 ArenaMessageDefaultTypeInternal _ArenaMessage_default_instance_;
}  // namespace proto2_arena_unittest
static ::_pb::Metadata file_level_metadata_google_2fprotobuf_2funittest_5farena_2eproto[2];
static constexpr const ::_pb::EnumDescriptor**
    file_level_enum_descriptors_google_2fprotobuf_2funittest_5farena_2eproto = nullptr;
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_google_2fprotobuf_2funittest_5farena_2eproto = nullptr;
const ::uint32_t TableStruct_google_2fprotobuf_2funittest_5farena_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(
    protodesc_cold) = {
    PROTOBUF_FIELD_OFFSET(::proto2_arena_unittest::NestedMessage, _impl_._has_bits_),
    PROTOBUF_FIELD_OFFSET(::proto2_arena_unittest::NestedMessage, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    ~0u,  // no _inlined_string_donated_
    ~0u,  // no _split_
    ~0u,  // no sizeof(Split)
    PROTOBUF_FIELD_OFFSET(::proto2_arena_unittest::NestedMessage, _impl_.d_),
    0,
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::proto2_arena_unittest::ArenaMessage, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    ~0u,  // no _inlined_string_donated_
    ~0u,  // no _split_
    ~0u,  // no sizeof(Split)
    PROTOBUF_FIELD_OFFSET(::proto2_arena_unittest::ArenaMessage, _impl_.repeated_nested_message_),
};

static const ::_pbi::MigrationSchema
    schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
        {0, 9, -1, sizeof(::proto2_arena_unittest::NestedMessage)},
        {10, -1, -1, sizeof(::proto2_arena_unittest::ArenaMessage)},
};

static const ::_pb::Message* const file_default_instances[] = {
    &::proto2_arena_unittest::_NestedMessage_default_instance_._instance,
    &::proto2_arena_unittest::_ArenaMessage_default_instance_._instance,
};
const char descriptor_table_protodef_google_2fprotobuf_2funittest_5farena_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
    "\n$google/protobuf/unittest_arena.proto\022\025"
    "proto2_arena_unittest\"\032\n\rNestedMessage\022\t"
    "\n\001d\030\001 \001(\005\"U\n\014ArenaMessage\022E\n\027repeated_ne"
    "sted_message\030\001 \003(\0132$.proto2_arena_unitte"
    "st.NestedMessageB\010\370\001\001\222\003\002\030\002b\010editionsj\00420"
    "23"
};
static ::absl::once_flag descriptor_table_google_2fprotobuf_2funittest_5farena_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_google_2fprotobuf_2funittest_5farena_2eproto = {
    false,
    false,
    202,
    descriptor_table_protodef_google_2fprotobuf_2funittest_5farena_2eproto,
    "google/protobuf/unittest_arena.proto",
    &descriptor_table_google_2fprotobuf_2funittest_5farena_2eproto_once,
    nullptr,
    0,
    2,
    schemas,
    file_default_instances,
    TableStruct_google_2fprotobuf_2funittest_5farena_2eproto::offsets,
    file_level_metadata_google_2fprotobuf_2funittest_5farena_2eproto,
    file_level_enum_descriptors_google_2fprotobuf_2funittest_5farena_2eproto,
    file_level_service_descriptors_google_2fprotobuf_2funittest_5farena_2eproto,
};

// This function exists to be marked as weak.
// It can significantly speed up compilation by breaking up LLVM's SCC
// in the .pb.cc translation units. Large translation units see a
// reduction of more than 35% of walltime for optimized builds. Without
// the weak attribute all the messages in the file, including all the
// vtables and everything they use become part of the same SCC through
// a cycle like:
// GetMetadata -> descriptor table -> default instances ->
//   vtables -> GetMetadata
// By adding a weak function here we break the connection from the
// individual vtables back into the descriptor table.
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_google_2fprotobuf_2funittest_5farena_2eproto_getter() {
  return &descriptor_table_google_2fprotobuf_2funittest_5farena_2eproto;
}
// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2
static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_google_2fprotobuf_2funittest_5farena_2eproto(&descriptor_table_google_2fprotobuf_2funittest_5farena_2eproto);
namespace proto2_arena_unittest {
// ===================================================================

class NestedMessage::_Internal {
 public:
  using HasBits = decltype(std::declval<NestedMessage>()._impl_._has_bits_);
  static constexpr ::int32_t kHasBitsOffset =
    8 * PROTOBUF_FIELD_OFFSET(NestedMessage, _impl_._has_bits_);
  static void set_has_d(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

NestedMessage::NestedMessage(::google::protobuf::Arena* arena)
    : ::google::protobuf::Message(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:proto2_arena_unittest.NestedMessage)
}
NestedMessage::NestedMessage(const NestedMessage& from)
    : ::google::protobuf::Message(), _impl_(from._impl_) {
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:proto2_arena_unittest.NestedMessage)
}
inline void NestedMessage::SharedCtor(::_pb::Arena* arena) {
  (void)arena;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){},
      /*decltype(_impl_._cached_size_)*/ {},
      decltype(_impl_.d_){0},
  };
}
NestedMessage::~NestedMessage() {
  // @@protoc_insertion_point(destructor:proto2_arena_unittest.NestedMessage)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void NestedMessage::SharedDtor() {
  ABSL_DCHECK(GetArenaForAllocation() == nullptr);
}
void NestedMessage::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

PROTOBUF_NOINLINE void NestedMessage::Clear() {
// @@protoc_insertion_point(message_clear_start:proto2_arena_unittest.NestedMessage)
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.d_ = 0;
  _impl_._has_bits_.Clear();
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

const char* NestedMessage::_InternalParse(
    const char* ptr, ::_pbi::ParseContext* ctx) {
  ptr = ::_pbi::TcParser::ParseLoop(this, ptr, ctx, &_table_.header);
  return ptr;
}


PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<0, 1, 0, 0, 2> NestedMessage::_table_ = {
  {
    PROTOBUF_FIELD_OFFSET(NestedMessage, _impl_._has_bits_),
    0, // no _extensions_
    1, 0,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967294,  // skipmap
    offsetof(decltype(_table_), field_entries),
    1,  // num_field_entries
    0,  // num_aux_entries
    offsetof(decltype(_table_), field_names),  // no aux_entries
    &_NestedMessage_default_instance_._instance,
    ::_pbi::TcParser::GenericFallback,  // fallback
  }, {{
    // int32 d = 1;
    {::_pbi::TcParser::SingularVarintNoZag1<::uint32_t, offsetof(NestedMessage, _impl_.d_), 0>(),
     {8, 0, 0, PROTOBUF_FIELD_OFFSET(NestedMessage, _impl_.d_)}},
  }}, {{
    65535, 65535
  }}, {{
    // int32 d = 1;
    {PROTOBUF_FIELD_OFFSET(NestedMessage, _impl_.d_), _Internal::kHasBitsOffset + 0, 0,
    (0 | ::_fl::kFcOptional | ::_fl::kInt32)},
  }},
  // no aux_entries
  {{
  }},
};

::uint8_t* NestedMessage::_InternalSerialize(
    ::uint8_t* target,
    ::google::protobuf::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:proto2_arena_unittest.NestedMessage)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  cached_has_bits = _impl_._has_bits_[0];
  // int32 d = 1;
  if (cached_has_bits & 0x00000001u) {
    target = ::google::protobuf::internal::WireFormatLite::
        WriteInt32ToArrayWithField<1>(
            stream, this->_internal_d(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:proto2_arena_unittest.NestedMessage)
  return target;
}

::size_t NestedMessage::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:proto2_arena_unittest.NestedMessage)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // int32 d = 1;
  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(
        this->_internal_d());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::google::protobuf::Message::ClassData NestedMessage::_class_data_ = {
    ::google::protobuf::Message::CopyWithSourceCheck,
    NestedMessage::MergeImpl
};
const ::google::protobuf::Message::ClassData*NestedMessage::GetClassData() const { return &_class_data_; }


void NestedMessage::MergeImpl(::google::protobuf::Message& to_msg, const ::google::protobuf::Message& from_msg) {
  auto* const _this = static_cast<NestedMessage*>(&to_msg);
  auto& from = static_cast<const NestedMessage&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:proto2_arena_unittest.NestedMessage)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if ((from._impl_._has_bits_[0] & 0x00000001u) != 0) {
    _this->_internal_set_d(from._internal_d());
  }
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void NestedMessage::CopyFrom(const NestedMessage& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:proto2_arena_unittest.NestedMessage)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

PROTOBUF_NOINLINE bool NestedMessage::IsInitialized() const {
  return true;
}

void NestedMessage::InternalSwap(NestedMessage* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_._has_bits_[0], other->_impl_._has_bits_[0]);
        swap(_impl_.d_, other->_impl_.d_);
}

::google::protobuf::Metadata NestedMessage::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_google_2fprotobuf_2funittest_5farena_2eproto_getter, &descriptor_table_google_2fprotobuf_2funittest_5farena_2eproto_once,
      file_level_metadata_google_2fprotobuf_2funittest_5farena_2eproto[0]);
}
// ===================================================================

class ArenaMessage::_Internal {
 public:
};

ArenaMessage::ArenaMessage(::google::protobuf::Arena* arena)
    : ::google::protobuf::Message(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:proto2_arena_unittest.ArenaMessage)
}
ArenaMessage::ArenaMessage(const ArenaMessage& from) : ::google::protobuf::Message() {
  ArenaMessage* const _this = this;
  (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.repeated_nested_message_){from._impl_.repeated_nested_message_},
      /*decltype(_impl_._cached_size_)*/ {},
  };
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);

  // @@protoc_insertion_point(copy_constructor:proto2_arena_unittest.ArenaMessage)
}
inline void ArenaMessage::SharedCtor(::_pb::Arena* arena) {
  (void)arena;
  new (&_impl_) Impl_{
      decltype(_impl_.repeated_nested_message_){arena},
      /*decltype(_impl_._cached_size_)*/ {},
  };
}
ArenaMessage::~ArenaMessage() {
  // @@protoc_insertion_point(destructor:proto2_arena_unittest.ArenaMessage)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void ArenaMessage::SharedDtor() {
  ABSL_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.repeated_nested_message_.~RepeatedPtrField();
}
void ArenaMessage::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

PROTOBUF_NOINLINE void ArenaMessage::Clear() {
// @@protoc_insertion_point(message_clear_start:proto2_arena_unittest.ArenaMessage)
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _internal_mutable_repeated_nested_message()->Clear();
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

const char* ArenaMessage::_InternalParse(
    const char* ptr, ::_pbi::ParseContext* ctx) {
  ptr = ::_pbi::TcParser::ParseLoop(this, ptr, ctx, &_table_.header);
  return ptr;
}


PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<0, 1, 1, 0, 2> ArenaMessage::_table_ = {
  {
    0,  // no _has_bits_
    0, // no _extensions_
    1, 0,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967294,  // skipmap
    offsetof(decltype(_table_), field_entries),
    1,  // num_field_entries
    1,  // num_aux_entries
    offsetof(decltype(_table_), aux_entries),
    &_ArenaMessage_default_instance_._instance,
    ::_pbi::TcParser::GenericFallback,  // fallback
  }, {{
    // repeated .proto2_arena_unittest.NestedMessage repeated_nested_message = 1;
    {::_pbi::TcParser::FastMtR1,
     {10, 63, 0, PROTOBUF_FIELD_OFFSET(ArenaMessage, _impl_.repeated_nested_message_)}},
  }}, {{
    65535, 65535
  }}, {{
    // repeated .proto2_arena_unittest.NestedMessage repeated_nested_message = 1;
    {PROTOBUF_FIELD_OFFSET(ArenaMessage, _impl_.repeated_nested_message_), 0, 0,
    (0 | ::_fl::kFcRepeated | ::_fl::kMessage | ::_fl::kTvTable)},
  }}, {{
    {::_pbi::TcParser::GetTable<::proto2_arena_unittest::NestedMessage>()},
  }}, {{
  }},
};

::uint8_t* ArenaMessage::_InternalSerialize(
    ::uint8_t* target,
    ::google::protobuf::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:proto2_arena_unittest.ArenaMessage)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  // repeated .proto2_arena_unittest.NestedMessage repeated_nested_message = 1;
  for (unsigned i = 0,
      n = static_cast<unsigned>(this->_internal_repeated_nested_message_size()); i < n; i++) {
    const auto& repfield = this->_internal_repeated_nested_message().Get(i);
    target = ::google::protobuf::internal::WireFormatLite::
        InternalWriteMessage(1, repfield, repfield.GetCachedSize(), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:proto2_arena_unittest.ArenaMessage)
  return target;
}

::size_t ArenaMessage::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:proto2_arena_unittest.ArenaMessage)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated .proto2_arena_unittest.NestedMessage repeated_nested_message = 1;
  total_size += 1UL * this->_internal_repeated_nested_message_size();
  for (const auto& msg : this->_internal_repeated_nested_message()) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSize(msg);
  }
  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::google::protobuf::Message::ClassData ArenaMessage::_class_data_ = {
    ::google::protobuf::Message::CopyWithSourceCheck,
    ArenaMessage::MergeImpl
};
const ::google::protobuf::Message::ClassData*ArenaMessage::GetClassData() const { return &_class_data_; }


void ArenaMessage::MergeImpl(::google::protobuf::Message& to_msg, const ::google::protobuf::Message& from_msg) {
  auto* const _this = static_cast<ArenaMessage*>(&to_msg);
  auto& from = static_cast<const ArenaMessage&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:proto2_arena_unittest.ArenaMessage)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  _this->_internal_mutable_repeated_nested_message()->MergeFrom(from._internal_repeated_nested_message());
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void ArenaMessage::CopyFrom(const ArenaMessage& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:proto2_arena_unittest.ArenaMessage)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

PROTOBUF_NOINLINE bool ArenaMessage::IsInitialized() const {
  return true;
}

void ArenaMessage::InternalSwap(ArenaMessage* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  _impl_.repeated_nested_message_.InternalSwap(&other->_impl_.repeated_nested_message_);
}

::google::protobuf::Metadata ArenaMessage::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_google_2fprotobuf_2funittest_5farena_2eproto_getter, &descriptor_table_google_2fprotobuf_2funittest_5farena_2eproto_once,
      file_level_metadata_google_2fprotobuf_2funittest_5farena_2eproto[1]);
}
// @@protoc_insertion_point(namespace_scope)
}  // namespace proto2_arena_unittest
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
#include "google/protobuf/port_undef.inc"
