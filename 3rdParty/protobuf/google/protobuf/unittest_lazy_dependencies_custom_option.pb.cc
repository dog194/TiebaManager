// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/protobuf/unittest_lazy_dependencies_custom_option.proto

#include "google/protobuf/unittest_lazy_dependencies_custom_option.pb.h"

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
namespace protobuf_unittest {
namespace lazy_imports {
        template <typename>
PROTOBUF_CONSTEXPR LazyMessage::LazyMessage(::_pbi::ConstantInitialized)
    : _impl_{
      /*decltype(_impl_._has_bits_)*/ {},
      /*decltype(_impl_._cached_size_)*/ {},
      /*decltype(_impl_.a_)*/ 0,
    } {}
struct LazyMessageDefaultTypeInternal {
  PROTOBUF_CONSTEXPR LazyMessageDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~LazyMessageDefaultTypeInternal() {}
  union {
    LazyMessage _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 LazyMessageDefaultTypeInternal _LazyMessage_default_instance_;
}  // namespace lazy_imports
}  // namespace protobuf_unittest
static ::_pb::Metadata file_level_metadata_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto[1];
static constexpr const ::_pb::EnumDescriptor**
    file_level_enum_descriptors_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto = nullptr;
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto = nullptr;
const ::uint32_t TableStruct_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(
    protodesc_cold) = {
    PROTOBUF_FIELD_OFFSET(::protobuf_unittest::lazy_imports::LazyMessage, _impl_._has_bits_),
    PROTOBUF_FIELD_OFFSET(::protobuf_unittest::lazy_imports::LazyMessage, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    ~0u,  // no _inlined_string_donated_
    ~0u,  // no _split_
    ~0u,  // no sizeof(Split)
    PROTOBUF_FIELD_OFFSET(::protobuf_unittest::lazy_imports::LazyMessage, _impl_.a_),
    0,
};

static const ::_pbi::MigrationSchema
    schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
        {0, 9, -1, sizeof(::protobuf_unittest::lazy_imports::LazyMessage)},
};

static const ::_pb::Message* const file_default_instances[] = {
    &::protobuf_unittest::lazy_imports::_LazyMessage_default_instance_._instance,
};
const char descriptor_table_protodef_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
    "\n>google/protobuf/unittest_lazy_dependen"
    "cies_custom_option.proto\022\036protobuf_unitt"
    "est.lazy_imports\032 google/protobuf/descri"
    "ptor.proto\0325google/protobuf/unittest_laz"
    "y_dependencies_enum.proto\"\030\n\013LazyMessage"
    "\022\t\n\001a\030\001 \001(\005:s\n\020lazy_enum_option\022\037.google"
    ".protobuf.MessageOptions\030\357\237\213B \001(\0162(.prot"
    "obuf_unittest.lazy_imports.LazyEnum:\013LAZ"
    "Y_ENUM_1B4B$UnittestLazyImportsCustomOpt"
    "ionProtoH\001\200\001\001\210\001\001\220\001\001\370\001\001b\010editionsj\0042023"
};
static const ::_pbi::DescriptorTable* const descriptor_table_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto_deps[2] =
    {
        &::descriptor_table_google_2fprotobuf_2fdescriptor_2eproto,
        &::descriptor_table_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fenum_2eproto,
};
static ::absl::once_flag descriptor_table_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto = {
    false,
    false,
    398,
    descriptor_table_protodef_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto,
    "google/protobuf/unittest_lazy_dependencies_custom_option.proto",
    &descriptor_table_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto_once,
    descriptor_table_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto_deps,
    2,
    1,
    schemas,
    file_default_instances,
    TableStruct_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto::offsets,
    file_level_metadata_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto,
    file_level_enum_descriptors_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto,
    file_level_service_descriptors_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto,
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
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto_getter() {
  return &descriptor_table_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto;
}
// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2
static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto(&descriptor_table_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto);
namespace protobuf_unittest {
namespace lazy_imports {
// ===================================================================

class LazyMessage::_Internal {
 public:
  using HasBits = decltype(std::declval<LazyMessage>()._impl_._has_bits_);
  static constexpr ::int32_t kHasBitsOffset =
    8 * PROTOBUF_FIELD_OFFSET(LazyMessage, _impl_._has_bits_);
  static void set_has_a(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

LazyMessage::LazyMessage(::google::protobuf::Arena* arena)
    : ::google::protobuf::Message(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:protobuf_unittest.lazy_imports.LazyMessage)
}
LazyMessage::LazyMessage(const LazyMessage& from)
    : ::google::protobuf::Message(), _impl_(from._impl_) {
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:protobuf_unittest.lazy_imports.LazyMessage)
}
inline void LazyMessage::SharedCtor(::_pb::Arena* arena) {
  (void)arena;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){},
      /*decltype(_impl_._cached_size_)*/ {},
      decltype(_impl_.a_){0},
  };
}
LazyMessage::~LazyMessage() {
  // @@protoc_insertion_point(destructor:protobuf_unittest.lazy_imports.LazyMessage)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void LazyMessage::SharedDtor() {
  ABSL_DCHECK(GetArenaForAllocation() == nullptr);
}
void LazyMessage::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

PROTOBUF_NOINLINE void LazyMessage::Clear() {
// @@protoc_insertion_point(message_clear_start:protobuf_unittest.lazy_imports.LazyMessage)
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.a_ = 0;
  _impl_._has_bits_.Clear();
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

const char* LazyMessage::_InternalParse(
    const char* ptr, ::_pbi::ParseContext* ctx) {
  ptr = ::_pbi::TcParser::ParseLoop(this, ptr, ctx, &_table_.header);
  return ptr;
}


PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<0, 1, 0, 0, 2> LazyMessage::_table_ = {
  {
    PROTOBUF_FIELD_OFFSET(LazyMessage, _impl_._has_bits_),
    0, // no _extensions_
    1, 0,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967294,  // skipmap
    offsetof(decltype(_table_), field_entries),
    1,  // num_field_entries
    0,  // num_aux_entries
    offsetof(decltype(_table_), field_names),  // no aux_entries
    &_LazyMessage_default_instance_._instance,
    ::_pbi::TcParser::GenericFallback,  // fallback
  }, {{
    // int32 a = 1;
    {::_pbi::TcParser::SingularVarintNoZag1<::uint32_t, offsetof(LazyMessage, _impl_.a_), 0>(),
     {8, 0, 0, PROTOBUF_FIELD_OFFSET(LazyMessage, _impl_.a_)}},
  }}, {{
    65535, 65535
  }}, {{
    // int32 a = 1;
    {PROTOBUF_FIELD_OFFSET(LazyMessage, _impl_.a_), _Internal::kHasBitsOffset + 0, 0,
    (0 | ::_fl::kFcOptional | ::_fl::kInt32)},
  }},
  // no aux_entries
  {{
  }},
};

::uint8_t* LazyMessage::_InternalSerialize(
    ::uint8_t* target,
    ::google::protobuf::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:protobuf_unittest.lazy_imports.LazyMessage)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  cached_has_bits = _impl_._has_bits_[0];
  // int32 a = 1;
  if (cached_has_bits & 0x00000001u) {
    target = ::google::protobuf::internal::WireFormatLite::
        WriteInt32ToArrayWithField<1>(
            stream, this->_internal_a(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:protobuf_unittest.lazy_imports.LazyMessage)
  return target;
}

::size_t LazyMessage::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:protobuf_unittest.lazy_imports.LazyMessage)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // int32 a = 1;
  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(
        this->_internal_a());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::google::protobuf::Message::ClassData LazyMessage::_class_data_ = {
    ::google::protobuf::Message::CopyWithSourceCheck,
    LazyMessage::MergeImpl
};
const ::google::protobuf::Message::ClassData*LazyMessage::GetClassData() const { return &_class_data_; }


void LazyMessage::MergeImpl(::google::protobuf::Message& to_msg, const ::google::protobuf::Message& from_msg) {
  auto* const _this = static_cast<LazyMessage*>(&to_msg);
  auto& from = static_cast<const LazyMessage&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:protobuf_unittest.lazy_imports.LazyMessage)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if ((from._impl_._has_bits_[0] & 0x00000001u) != 0) {
    _this->_internal_set_a(from._internal_a());
  }
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void LazyMessage::CopyFrom(const LazyMessage& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:protobuf_unittest.lazy_imports.LazyMessage)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

PROTOBUF_NOINLINE bool LazyMessage::IsInitialized() const {
  return true;
}

void LazyMessage::InternalSwap(LazyMessage* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_._has_bits_[0], other->_impl_._has_bits_[0]);
        swap(_impl_.a_, other->_impl_.a_);
}

::google::protobuf::Metadata LazyMessage::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto_getter, &descriptor_table_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto_once,
      file_level_metadata_google_2fprotobuf_2funittest_5flazy_5fdependencies_5fcustom_5foption_2eproto[0]);
}
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 ::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::MessageOptions,
    ::google::protobuf::internal::EnumTypeTraits< ::protobuf_unittest::lazy_imports::LazyEnum, ::protobuf_unittest::lazy_imports::LazyEnum_IsValid>, 14, false>
  lazy_enum_option(kLazyEnumOptionFieldNumber, static_cast< ::protobuf_unittest::lazy_imports::LazyEnum >(1), nullptr);
// @@protoc_insertion_point(namespace_scope)
}  // namespace lazy_imports
}  // namespace protobuf_unittest
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
#include "google/protobuf/port_undef.inc"
