// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: PbPageResIdl.proto

#include "PbPageResIdl.pb.h"

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
        template <typename>
PROTOBUF_CONSTEXPR PbPageResIdl_DataRes::PbPageResIdl_DataRes(::_pbi::ConstantInitialized)
    : _impl_{
      /*decltype(_impl_._has_bits_)*/ {},
      /*decltype(_impl_._cached_size_)*/ {},
      /*decltype(_impl_.post_list_)*/ {},
      /*decltype(_impl_.user_list_)*/ {},
      /*decltype(_impl_.forum_)*/ nullptr,
      /*decltype(_impl_.page_)*/ nullptr,
      /*decltype(_impl_.thread_)*/ nullptr,
    } {}
struct PbPageResIdl_DataResDefaultTypeInternal {
  PROTOBUF_CONSTEXPR PbPageResIdl_DataResDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~PbPageResIdl_DataResDefaultTypeInternal() {}
  union {
    PbPageResIdl_DataRes _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 PbPageResIdl_DataResDefaultTypeInternal _PbPageResIdl_DataRes_default_instance_;
        template <typename>
PROTOBUF_CONSTEXPR PbPageResIdl::PbPageResIdl(::_pbi::ConstantInitialized)
    : _impl_{
      /*decltype(_impl_._has_bits_)*/ {},
      /*decltype(_impl_._cached_size_)*/ {},
      /*decltype(_impl_.error_)*/ nullptr,
      /*decltype(_impl_.data_)*/ nullptr,
    } {}
struct PbPageResIdlDefaultTypeInternal {
  PROTOBUF_CONSTEXPR PbPageResIdlDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~PbPageResIdlDefaultTypeInternal() {}
  union {
    PbPageResIdl _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 PbPageResIdlDefaultTypeInternal _PbPageResIdl_default_instance_;
static ::_pb::Metadata file_level_metadata_PbPageResIdl_2eproto[2];
static constexpr const ::_pb::EnumDescriptor**
    file_level_enum_descriptors_PbPageResIdl_2eproto = nullptr;
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_PbPageResIdl_2eproto = nullptr;
const ::uint32_t TableStruct_PbPageResIdl_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(
    protodesc_cold) = {
    PROTOBUF_FIELD_OFFSET(::PbPageResIdl_DataRes, _impl_._has_bits_),
    PROTOBUF_FIELD_OFFSET(::PbPageResIdl_DataRes, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    ~0u,  // no _inlined_string_donated_
    ~0u,  // no _split_
    ~0u,  // no sizeof(Split)
    PROTOBUF_FIELD_OFFSET(::PbPageResIdl_DataRes, _impl_.forum_),
    PROTOBUF_FIELD_OFFSET(::PbPageResIdl_DataRes, _impl_.page_),
    PROTOBUF_FIELD_OFFSET(::PbPageResIdl_DataRes, _impl_.post_list_),
    PROTOBUF_FIELD_OFFSET(::PbPageResIdl_DataRes, _impl_.thread_),
    PROTOBUF_FIELD_OFFSET(::PbPageResIdl_DataRes, _impl_.user_list_),
    0,
    1,
    ~0u,
    2,
    ~0u,
    PROTOBUF_FIELD_OFFSET(::PbPageResIdl, _impl_._has_bits_),
    PROTOBUF_FIELD_OFFSET(::PbPageResIdl, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    ~0u,  // no _inlined_string_donated_
    ~0u,  // no _split_
    ~0u,  // no sizeof(Split)
    PROTOBUF_FIELD_OFFSET(::PbPageResIdl, _impl_.error_),
    PROTOBUF_FIELD_OFFSET(::PbPageResIdl, _impl_.data_),
    0,
    1,
};

static const ::_pbi::MigrationSchema
    schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
        {0, 13, -1, sizeof(::PbPageResIdl_DataRes)},
        {18, 28, -1, sizeof(::PbPageResIdl)},
};

static const ::_pb::Message* const file_default_instances[] = {
    &::_PbPageResIdl_DataRes_default_instance_._instance,
    &::_PbPageResIdl_default_instance_._instance,
};
const char descriptor_table_protodef_PbPageResIdl_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
    "\n\022PbPageResIdl.proto\032\021SimpleForum.proto\032"
    "\nPage.proto\032\nPost.proto\032\020ThreadInfo.prot"
    "o\032\nUser.proto\032\013Error.proto\"\331\001\n\014PbPageRes"
    "Idl\022\025\n\005error\030\001 \001(\0132\006.Error\022#\n\004data\030\002 \001(\013"
    "2\025.PbPageResIdl.DataRes\032\214\001\n\007DataRes\022\033\n\005f"
    "orum\030\002 \001(\0132\014.SimpleForum\022\023\n\004page\030\003 \001(\0132\005"
    ".Page\022\030\n\tpost_list\030\006 \003(\0132\005.Post\022\033\n\006threa"
    "d\030\010 \001(\0132\013.ThreadInfo\022\030\n\tuser_list\030\r \003(\0132"
    "\005.Userb\006proto3"
};
static const ::_pbi::DescriptorTable* const descriptor_table_PbPageResIdl_2eproto_deps[6] =
    {
        &::descriptor_table_Error_2eproto,
        &::descriptor_table_Page_2eproto,
        &::descriptor_table_Post_2eproto,
        &::descriptor_table_SimpleForum_2eproto,
        &::descriptor_table_ThreadInfo_2eproto,
        &::descriptor_table_User_2eproto,
};
static ::absl::once_flag descriptor_table_PbPageResIdl_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_PbPageResIdl_2eproto = {
    false,
    false,
    334,
    descriptor_table_protodef_PbPageResIdl_2eproto,
    "PbPageResIdl.proto",
    &descriptor_table_PbPageResIdl_2eproto_once,
    descriptor_table_PbPageResIdl_2eproto_deps,
    6,
    2,
    schemas,
    file_default_instances,
    TableStruct_PbPageResIdl_2eproto::offsets,
    file_level_metadata_PbPageResIdl_2eproto,
    file_level_enum_descriptors_PbPageResIdl_2eproto,
    file_level_service_descriptors_PbPageResIdl_2eproto,
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
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_PbPageResIdl_2eproto_getter() {
  return &descriptor_table_PbPageResIdl_2eproto;
}
// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2
static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_PbPageResIdl_2eproto(&descriptor_table_PbPageResIdl_2eproto);
// ===================================================================

class PbPageResIdl_DataRes::_Internal {
 public:
  using HasBits = decltype(std::declval<PbPageResIdl_DataRes>()._impl_._has_bits_);
  static constexpr ::int32_t kHasBitsOffset =
    8 * PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_._has_bits_);
  static const ::SimpleForum& forum(const PbPageResIdl_DataRes* msg);
  static void set_has_forum(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static const ::Page& page(const PbPageResIdl_DataRes* msg);
  static void set_has_page(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static const ::ThreadInfo& thread(const PbPageResIdl_DataRes* msg);
  static void set_has_thread(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
};

const ::SimpleForum& PbPageResIdl_DataRes::_Internal::forum(const PbPageResIdl_DataRes* msg) {
  return *msg->_impl_.forum_;
}
const ::Page& PbPageResIdl_DataRes::_Internal::page(const PbPageResIdl_DataRes* msg) {
  return *msg->_impl_.page_;
}
const ::ThreadInfo& PbPageResIdl_DataRes::_Internal::thread(const PbPageResIdl_DataRes* msg) {
  return *msg->_impl_.thread_;
}
void PbPageResIdl_DataRes::clear_forum() {
  if (_impl_.forum_ != nullptr) _impl_.forum_->Clear();
  _impl_._has_bits_[0] &= ~0x00000001u;
}
void PbPageResIdl_DataRes::clear_page() {
  if (_impl_.page_ != nullptr) _impl_.page_->Clear();
  _impl_._has_bits_[0] &= ~0x00000002u;
}
void PbPageResIdl_DataRes::clear_post_list() {
  _internal_mutable_post_list()->Clear();
}
void PbPageResIdl_DataRes::clear_thread() {
  if (_impl_.thread_ != nullptr) _impl_.thread_->Clear();
  _impl_._has_bits_[0] &= ~0x00000004u;
}
void PbPageResIdl_DataRes::clear_user_list() {
  _internal_mutable_user_list()->Clear();
}
PbPageResIdl_DataRes::PbPageResIdl_DataRes(::google::protobuf::Arena* arena)
    : ::google::protobuf::Message(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:PbPageResIdl.DataRes)
}
PbPageResIdl_DataRes::PbPageResIdl_DataRes(const PbPageResIdl_DataRes& from) : ::google::protobuf::Message() {
  PbPageResIdl_DataRes* const _this = this;
  (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){from._impl_._has_bits_},
      /*decltype(_impl_._cached_size_)*/ {},
      decltype(_impl_.post_list_){from._impl_.post_list_},
      decltype(_impl_.user_list_){from._impl_.user_list_},
      decltype(_impl_.forum_){nullptr},
      decltype(_impl_.page_){nullptr},
      decltype(_impl_.thread_){nullptr},
  };
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);
  if ((from._impl_._has_bits_[0] & 0x00000001u) != 0) {
    _this->_impl_.forum_ = new ::SimpleForum(*from._impl_.forum_);
  }
  if ((from._impl_._has_bits_[0] & 0x00000002u) != 0) {
    _this->_impl_.page_ = new ::Page(*from._impl_.page_);
  }
  if ((from._impl_._has_bits_[0] & 0x00000004u) != 0) {
    _this->_impl_.thread_ = new ::ThreadInfo(*from._impl_.thread_);
  }

  // @@protoc_insertion_point(copy_constructor:PbPageResIdl.DataRes)
}
inline void PbPageResIdl_DataRes::SharedCtor(::_pb::Arena* arena) {
  (void)arena;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){},
      /*decltype(_impl_._cached_size_)*/ {},
      decltype(_impl_.post_list_){arena},
      decltype(_impl_.user_list_){arena},
      decltype(_impl_.forum_){nullptr},
      decltype(_impl_.page_){nullptr},
      decltype(_impl_.thread_){nullptr},
  };
}
PbPageResIdl_DataRes::~PbPageResIdl_DataRes() {
  // @@protoc_insertion_point(destructor:PbPageResIdl.DataRes)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void PbPageResIdl_DataRes::SharedDtor() {
  ABSL_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.post_list_.~RepeatedPtrField();
  _impl_.user_list_.~RepeatedPtrField();
  if (this != internal_default_instance()) delete _impl_.forum_;
  if (this != internal_default_instance()) delete _impl_.page_;
  if (this != internal_default_instance()) delete _impl_.thread_;
}
void PbPageResIdl_DataRes::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

PROTOBUF_NOINLINE void PbPageResIdl_DataRes::Clear() {
// @@protoc_insertion_point(message_clear_start:PbPageResIdl.DataRes)
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _internal_mutable_post_list()->Clear();
  _internal_mutable_user_list()->Clear();
  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    if (cached_has_bits & 0x00000001u) {
      ABSL_DCHECK(_impl_.forum_ != nullptr);
      _impl_.forum_->Clear();
    }
    if (cached_has_bits & 0x00000002u) {
      ABSL_DCHECK(_impl_.page_ != nullptr);
      _impl_.page_->Clear();
    }
    if (cached_has_bits & 0x00000004u) {
      ABSL_DCHECK(_impl_.thread_ != nullptr);
      _impl_.thread_->Clear();
    }
  }
  _impl_._has_bits_.Clear();
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

const char* PbPageResIdl_DataRes::_InternalParse(
    const char* ptr, ::_pbi::ParseContext* ctx) {
  ptr = ::_pbi::TcParser::ParseLoop(this, ptr, ctx, &_table_.header);
  return ptr;
}


PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<3, 5, 5, 0, 2> PbPageResIdl_DataRes::_table_ = {
  {
    PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_._has_bits_),
    0, // no _extensions_
    13, 56,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294963033,  // skipmap
    offsetof(decltype(_table_), field_entries),
    5,  // num_field_entries
    5,  // num_aux_entries
    offsetof(decltype(_table_), aux_entries),
    &_PbPageResIdl_DataRes_default_instance_._instance,
    ::_pbi::TcParser::GenericFallback,  // fallback
  }, {{
    // .ThreadInfo thread = 8;
    {::_pbi::TcParser::FastMtS1,
     {66, 2, 3, PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_.thread_)}},
    {::_pbi::TcParser::MiniParse, {}},
    // .SimpleForum forum = 2;
    {::_pbi::TcParser::FastMtS1,
     {18, 0, 0, PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_.forum_)}},
    // .Page page = 3;
    {::_pbi::TcParser::FastMtS1,
     {26, 1, 1, PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_.page_)}},
    {::_pbi::TcParser::MiniParse, {}},
    // repeated .User user_list = 13;
    {::_pbi::TcParser::FastMtR1,
     {106, 63, 4, PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_.user_list_)}},
    // repeated .Post post_list = 6;
    {::_pbi::TcParser::FastMtR1,
     {50, 63, 2, PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_.post_list_)}},
    {::_pbi::TcParser::MiniParse, {}},
  }}, {{
    65535, 65535
  }}, {{
    // .SimpleForum forum = 2;
    {PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_.forum_), _Internal::kHasBitsOffset + 0, 0,
    (0 | ::_fl::kFcOptional | ::_fl::kMessage | ::_fl::kTvTable)},
    // .Page page = 3;
    {PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_.page_), _Internal::kHasBitsOffset + 1, 1,
    (0 | ::_fl::kFcOptional | ::_fl::kMessage | ::_fl::kTvTable)},
    // repeated .Post post_list = 6;
    {PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_.post_list_), -1, 2,
    (0 | ::_fl::kFcRepeated | ::_fl::kMessage | ::_fl::kTvTable)},
    // .ThreadInfo thread = 8;
    {PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_.thread_), _Internal::kHasBitsOffset + 2, 3,
    (0 | ::_fl::kFcOptional | ::_fl::kMessage | ::_fl::kTvTable)},
    // repeated .User user_list = 13;
    {PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_.user_list_), -1, 4,
    (0 | ::_fl::kFcRepeated | ::_fl::kMessage | ::_fl::kTvTable)},
  }}, {{
    {::_pbi::TcParser::GetTable<::SimpleForum>()},
    {::_pbi::TcParser::GetTable<::Page>()},
    {::_pbi::TcParser::GetTable<::Post>()},
    {::_pbi::TcParser::GetTable<::ThreadInfo>()},
    {::_pbi::TcParser::GetTable<::User>()},
  }}, {{
  }},
};

::uint8_t* PbPageResIdl_DataRes::_InternalSerialize(
    ::uint8_t* target,
    ::google::protobuf::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:PbPageResIdl.DataRes)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  cached_has_bits = _impl_._has_bits_[0];
  // .SimpleForum forum = 2;
  if (cached_has_bits & 0x00000001u) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessage(2, _Internal::forum(this),
        _Internal::forum(this).GetCachedSize(), target, stream);
  }

  // .Page page = 3;
  if (cached_has_bits & 0x00000002u) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessage(3, _Internal::page(this),
        _Internal::page(this).GetCachedSize(), target, stream);
  }

  // repeated .Post post_list = 6;
  for (unsigned i = 0,
      n = static_cast<unsigned>(this->_internal_post_list_size()); i < n; i++) {
    const auto& repfield = this->_internal_post_list().Get(i);
    target = ::google::protobuf::internal::WireFormatLite::
        InternalWriteMessage(6, repfield, repfield.GetCachedSize(), target, stream);
  }

  // .ThreadInfo thread = 8;
  if (cached_has_bits & 0x00000004u) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessage(8, _Internal::thread(this),
        _Internal::thread(this).GetCachedSize(), target, stream);
  }

  // repeated .User user_list = 13;
  for (unsigned i = 0,
      n = static_cast<unsigned>(this->_internal_user_list_size()); i < n; i++) {
    const auto& repfield = this->_internal_user_list().Get(i);
    target = ::google::protobuf::internal::WireFormatLite::
        InternalWriteMessage(13, repfield, repfield.GetCachedSize(), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:PbPageResIdl.DataRes)
  return target;
}

::size_t PbPageResIdl_DataRes::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:PbPageResIdl.DataRes)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated .Post post_list = 6;
  total_size += 1UL * this->_internal_post_list_size();
  for (const auto& msg : this->_internal_post_list()) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSize(msg);
  }
  // repeated .User user_list = 13;
  total_size += 1UL * this->_internal_user_list_size();
  for (const auto& msg : this->_internal_user_list()) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSize(msg);
  }
  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    // .SimpleForum forum = 2;
    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSize(
          *_impl_.forum_);
    }

    // .Page page = 3;
    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSize(
          *_impl_.page_);
    }

    // .ThreadInfo thread = 8;
    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSize(
          *_impl_.thread_);
    }

  }
  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::google::protobuf::Message::ClassData PbPageResIdl_DataRes::_class_data_ = {
    ::google::protobuf::Message::CopyWithSourceCheck,
    PbPageResIdl_DataRes::MergeImpl
};
const ::google::protobuf::Message::ClassData*PbPageResIdl_DataRes::GetClassData() const { return &_class_data_; }


void PbPageResIdl_DataRes::MergeImpl(::google::protobuf::Message& to_msg, const ::google::protobuf::Message& from_msg) {
  auto* const _this = static_cast<PbPageResIdl_DataRes*>(&to_msg);
  auto& from = static_cast<const PbPageResIdl_DataRes&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:PbPageResIdl.DataRes)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  _this->_internal_mutable_post_list()->MergeFrom(from._internal_post_list());
  _this->_internal_mutable_user_list()->MergeFrom(from._internal_user_list());
  cached_has_bits = from._impl_._has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    if (cached_has_bits & 0x00000001u) {
      _this->_internal_mutable_forum()->::SimpleForum::MergeFrom(
          from._internal_forum());
    }
    if (cached_has_bits & 0x00000002u) {
      _this->_internal_mutable_page()->::Page::MergeFrom(
          from._internal_page());
    }
    if (cached_has_bits & 0x00000004u) {
      _this->_internal_mutable_thread()->::ThreadInfo::MergeFrom(
          from._internal_thread());
    }
  }
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void PbPageResIdl_DataRes::CopyFrom(const PbPageResIdl_DataRes& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:PbPageResIdl.DataRes)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

PROTOBUF_NOINLINE bool PbPageResIdl_DataRes::IsInitialized() const {
  return true;
}

void PbPageResIdl_DataRes::InternalSwap(PbPageResIdl_DataRes* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_._has_bits_[0], other->_impl_._has_bits_[0]);
  _impl_.post_list_.InternalSwap(&other->_impl_.post_list_);
  _impl_.user_list_.InternalSwap(&other->_impl_.user_list_);
  ::google::protobuf::internal::memswap<
      PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_.thread_)
      + sizeof(PbPageResIdl_DataRes::_impl_.thread_)
      - PROTOBUF_FIELD_OFFSET(PbPageResIdl_DataRes, _impl_.forum_)>(
          reinterpret_cast<char*>(&_impl_.forum_),
          reinterpret_cast<char*>(&other->_impl_.forum_));
}

::google::protobuf::Metadata PbPageResIdl_DataRes::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_PbPageResIdl_2eproto_getter, &descriptor_table_PbPageResIdl_2eproto_once,
      file_level_metadata_PbPageResIdl_2eproto[0]);
}
// ===================================================================

class PbPageResIdl::_Internal {
 public:
  using HasBits = decltype(std::declval<PbPageResIdl>()._impl_._has_bits_);
  static constexpr ::int32_t kHasBitsOffset =
    8 * PROTOBUF_FIELD_OFFSET(PbPageResIdl, _impl_._has_bits_);
  static const ::Error& error(const PbPageResIdl* msg);
  static void set_has_error(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static const ::PbPageResIdl_DataRes& data(const PbPageResIdl* msg);
  static void set_has_data(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
};

const ::Error& PbPageResIdl::_Internal::error(const PbPageResIdl* msg) {
  return *msg->_impl_.error_;
}
const ::PbPageResIdl_DataRes& PbPageResIdl::_Internal::data(const PbPageResIdl* msg) {
  return *msg->_impl_.data_;
}
void PbPageResIdl::clear_error() {
  if (_impl_.error_ != nullptr) _impl_.error_->Clear();
  _impl_._has_bits_[0] &= ~0x00000001u;
}
PbPageResIdl::PbPageResIdl(::google::protobuf::Arena* arena)
    : ::google::protobuf::Message(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:PbPageResIdl)
}
PbPageResIdl::PbPageResIdl(const PbPageResIdl& from) : ::google::protobuf::Message() {
  PbPageResIdl* const _this = this;
  (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){from._impl_._has_bits_},
      /*decltype(_impl_._cached_size_)*/ {},
      decltype(_impl_.error_){nullptr},
      decltype(_impl_.data_){nullptr},
  };
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);
  if ((from._impl_._has_bits_[0] & 0x00000001u) != 0) {
    _this->_impl_.error_ = new ::Error(*from._impl_.error_);
  }
  if ((from._impl_._has_bits_[0] & 0x00000002u) != 0) {
    _this->_impl_.data_ = new ::PbPageResIdl_DataRes(*from._impl_.data_);
  }

  // @@protoc_insertion_point(copy_constructor:PbPageResIdl)
}
inline void PbPageResIdl::SharedCtor(::_pb::Arena* arena) {
  (void)arena;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){},
      /*decltype(_impl_._cached_size_)*/ {},
      decltype(_impl_.error_){nullptr},
      decltype(_impl_.data_){nullptr},
  };
}
PbPageResIdl::~PbPageResIdl() {
  // @@protoc_insertion_point(destructor:PbPageResIdl)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void PbPageResIdl::SharedDtor() {
  ABSL_DCHECK(GetArenaForAllocation() == nullptr);
  if (this != internal_default_instance()) delete _impl_.error_;
  if (this != internal_default_instance()) delete _impl_.data_;
}
void PbPageResIdl::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

PROTOBUF_NOINLINE void PbPageResIdl::Clear() {
// @@protoc_insertion_point(message_clear_start:PbPageResIdl)
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      ABSL_DCHECK(_impl_.error_ != nullptr);
      _impl_.error_->Clear();
    }
    if (cached_has_bits & 0x00000002u) {
      ABSL_DCHECK(_impl_.data_ != nullptr);
      _impl_.data_->Clear();
    }
  }
  _impl_._has_bits_.Clear();
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

const char* PbPageResIdl::_InternalParse(
    const char* ptr, ::_pbi::ParseContext* ctx) {
  ptr = ::_pbi::TcParser::ParseLoop(this, ptr, ctx, &_table_.header);
  return ptr;
}


PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<1, 2, 2, 0, 2> PbPageResIdl::_table_ = {
  {
    PROTOBUF_FIELD_OFFSET(PbPageResIdl, _impl_._has_bits_),
    0, // no _extensions_
    2, 8,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967292,  // skipmap
    offsetof(decltype(_table_), field_entries),
    2,  // num_field_entries
    2,  // num_aux_entries
    offsetof(decltype(_table_), aux_entries),
    &_PbPageResIdl_default_instance_._instance,
    ::_pbi::TcParser::GenericFallback,  // fallback
  }, {{
    // .PbPageResIdl.DataRes data = 2;
    {::_pbi::TcParser::FastMtS1,
     {18, 1, 1, PROTOBUF_FIELD_OFFSET(PbPageResIdl, _impl_.data_)}},
    // .Error error = 1;
    {::_pbi::TcParser::FastMtS1,
     {10, 0, 0, PROTOBUF_FIELD_OFFSET(PbPageResIdl, _impl_.error_)}},
  }}, {{
    65535, 65535
  }}, {{
    // .Error error = 1;
    {PROTOBUF_FIELD_OFFSET(PbPageResIdl, _impl_.error_), _Internal::kHasBitsOffset + 0, 0,
    (0 | ::_fl::kFcOptional | ::_fl::kMessage | ::_fl::kTvTable)},
    // .PbPageResIdl.DataRes data = 2;
    {PROTOBUF_FIELD_OFFSET(PbPageResIdl, _impl_.data_), _Internal::kHasBitsOffset + 1, 1,
    (0 | ::_fl::kFcOptional | ::_fl::kMessage | ::_fl::kTvTable)},
  }}, {{
    {::_pbi::TcParser::GetTable<::Error>()},
    {::_pbi::TcParser::GetTable<::PbPageResIdl_DataRes>()},
  }}, {{
  }},
};

::uint8_t* PbPageResIdl::_InternalSerialize(
    ::uint8_t* target,
    ::google::protobuf::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:PbPageResIdl)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  cached_has_bits = _impl_._has_bits_[0];
  // .Error error = 1;
  if (cached_has_bits & 0x00000001u) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessage(1, _Internal::error(this),
        _Internal::error(this).GetCachedSize(), target, stream);
  }

  // .PbPageResIdl.DataRes data = 2;
  if (cached_has_bits & 0x00000002u) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessage(2, _Internal::data(this),
        _Internal::data(this).GetCachedSize(), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:PbPageResIdl)
  return target;
}

::size_t PbPageResIdl::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:PbPageResIdl)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    // .Error error = 1;
    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSize(
          *_impl_.error_);
    }

    // .PbPageResIdl.DataRes data = 2;
    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSize(
          *_impl_.data_);
    }

  }
  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::google::protobuf::Message::ClassData PbPageResIdl::_class_data_ = {
    ::google::protobuf::Message::CopyWithSourceCheck,
    PbPageResIdl::MergeImpl
};
const ::google::protobuf::Message::ClassData*PbPageResIdl::GetClassData() const { return &_class_data_; }


void PbPageResIdl::MergeImpl(::google::protobuf::Message& to_msg, const ::google::protobuf::Message& from_msg) {
  auto* const _this = static_cast<PbPageResIdl*>(&to_msg);
  auto& from = static_cast<const PbPageResIdl&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:PbPageResIdl)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._impl_._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      _this->_internal_mutable_error()->::Error::MergeFrom(
          from._internal_error());
    }
    if (cached_has_bits & 0x00000002u) {
      _this->_internal_mutable_data()->::PbPageResIdl_DataRes::MergeFrom(
          from._internal_data());
    }
  }
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void PbPageResIdl::CopyFrom(const PbPageResIdl& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:PbPageResIdl)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

PROTOBUF_NOINLINE bool PbPageResIdl::IsInitialized() const {
  return true;
}

void PbPageResIdl::InternalSwap(PbPageResIdl* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_._has_bits_[0], other->_impl_._has_bits_[0]);
  ::google::protobuf::internal::memswap<
      PROTOBUF_FIELD_OFFSET(PbPageResIdl, _impl_.data_)
      + sizeof(PbPageResIdl::_impl_.data_)
      - PROTOBUF_FIELD_OFFSET(PbPageResIdl, _impl_.error_)>(
          reinterpret_cast<char*>(&_impl_.error_),
          reinterpret_cast<char*>(&other->_impl_.error_));
}

::google::protobuf::Metadata PbPageResIdl::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_PbPageResIdl_2eproto_getter, &descriptor_table_PbPageResIdl_2eproto_once,
      file_level_metadata_PbPageResIdl_2eproto[1]);
}
// @@protoc_insertion_point(namespace_scope)
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
#include "google/protobuf/port_undef.inc"
