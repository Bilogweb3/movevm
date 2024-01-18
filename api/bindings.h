/* (c) 2022 initia labs. Licensed under BUSL-1.1 */

#ifndef __LIBINITIAVM__
#define __LIBINITIAVM__

/* Generated with cbindgen:0.26.0 */

/* Warning, this file is autogenerated by cbindgen. Don't modify this manually. */

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>


enum CoverageOption {
  /**
   * Display a coverage summary for all modules in this package
   */
  CoverageOption_Summary = 0,
  /**
   * Display coverage information about the module against source code
   */
  CoverageOption_Source = 1,
  /**
   * Display coverage information about the module against disassembled bytecode
   */
  CoverageOption_Bytecode = 2,
};
typedef uint8_t CoverageOption;

enum ErrnoValue {
  ErrnoValue_Success = 0,
  ErrnoValue_Other = 1,
};
typedef int32_t ErrnoValue;

/**
 * This enum gives names to the status codes returned from Go callbacks to Rust.
 * The Go code will return one of these variants when returning.
 *
 * 0 means no error, all the other cases are some sort of error.
 *
 */
enum GoError {
  GoError_None = 0,
  /**
   * Go panicked for an unexpected reason.
   */
  GoError_Panic = 1,
  /**
   * Go received a bad argument from Rust
   */
  GoError_BadArgument = 2,
  /**
   * Error while trying to serialize data in Go code (typically json.Marshal)
   */
  GoError_CannotSerialize = 3,
  /**
   * An error happened during normal operation of a Go callback, which should be fed back to the contract
   */
  GoError_User = 4,
  /**
   * Unimplemented
   */
  GoError_Unimplemented = 5,
  /**
   * An error type that should never be created by us. It only serves as a fallback for the i32 to GoError conversion.
   */
  GoError_Other = -1,
};
typedef int32_t GoError;

typedef struct {

} vm_t;

/**
 * An optional Vector type that requires explicit creation and destruction
 * and can be sent via FFI.
 * It can be created from `Option<Vec<u8>>` and be converted into `Option<Vec<u8>>`.
 *
 * This type is always created in Rust and always dropped in Rust.
 * If Go code want to create it, it must instruct Rust to do so via the
 * [`new_unmanaged_vector`] FFI export. If Go code wants to consume its data,
 * it must create a copy and instruct Rust to destroy it via the
 * [`destroy_unmanaged_vector`] FFI export.
 *
 * An UnmanagedVector is immutable.
 *
 * ## Ownership
 *
 * Ownership is the right and the obligation to destroy an `UnmanagedVector`
 * exactly once. Both Rust and Go can create an `UnmanagedVector`, which gives
 * then ownership. Sometimes it is necessary to transfer ownership.
 *
 * ### Transfer ownership from Rust to Go
 *
 * When an `UnmanagedVector` was created in Rust using [`UnmanagedVector::new`], [`UnmanagedVector::default`]
 * or [`new_unmanaged_vector`], it can be passted to Go as a return value.
 * Rust then has no chance to destroy the vector anymore, so ownership is transferred to Go.
 * In Go, the data has to be copied to a garbage collected `[]byte`. Then the vector must be destroyed
 * using [`destroy_unmanaged_vector`].
 *
 * ### Transfer ownership from Go to Rust
 *
 * When Rust code calls into Go (using the vtable methods), return data or error messages must be created
 * in Go. This is done by calling [`new_unmanaged_vector`] from Go, which copies data into a newly created
 * `UnmanagedVector`. Since Go created it, it owns it. The ownership is then passed to Rust via the
 * mutable return value pointers. On the Rust side, the vector is destroyed using [`UnmanagedVector::consume`].
 *
 */
typedef struct {
  /**
   * True if and only if this is None. If this is true, the other fields must be ignored.
   */
  bool is_none;
  uint8_t *ptr;
  size_t len;
  size_t cap;
} UnmanagedVector;

/**
 * A view into an externally owned byte slice (Go `[]byte`).
 * Use this for the current call only. A view cannot be copied for safety reasons.
 * If you need a copy, use [`ByteSliceView::to_owned`].
 *
 * Go's nil value is fully supported, such that we can differentiate between nil and an empty slice.
 */
typedef struct {
  /**
   * True if and only if the byte slice is nil in Go. If this is true, the other fields must be ignored.
   */
  bool is_nil;
  const uint8_t *ptr;
  size_t len;
} ByteSliceView;

typedef struct {
  /**
   * Compile in 'dev' mode. The 'dev-addresses' and 'dev-dependencies' fields will be used if
   * this flag is set. This flag is useful for development of packages that expose named
   * addresses that are not set to a specific value.
   */
  bool dev_mode;
  /**
   * Compile in 'test' mode. The 'dev-addresses' and 'dev-dependencies' fields will be used
   * along with any code in the 'tests' directory.
   */
  bool test_mode;
  /**
   * Generate documentation for packages
   */
  bool generate_docs;
  /**
   * Generate ABIs for packages
   */
  bool generate_abis;
  /**
   * Installation directory for compiled artifacts. Defaults to current directory.
   */
  ByteSliceView install_dir;
  /**
   * Force recompilation of all packages
   */
  bool force_recompilation;
  /**
   * Only fetch dependency repos to MOVE_HOME
   */
  bool fetch_deps_only;
  /**
   * Skip fetching latest git dependencies
   */
  bool skip_fetch_latest_git_deps;
  /**
   * bytecode version. set 0 to unset and to use default
   */
  uint32_t bytecode_version;
} InitiaCompilerBuildConfig;

typedef struct {
  /**
   * Path to a package which the command should be run with respect to.
   */
  ByteSliceView package_path;
  /**
   * Print additional diagnostics if available.
   */
  bool verbose;
  /**
   * Package build options
   */
  InitiaCompilerBuildConfig build_config;
} InitiaCompilerArgument;

typedef struct {
  ByteSliceView module_name;
} InitiaCompilerCoverageBytecodeOption;

typedef struct {
  ByteSliceView module_name;
} InitiaCompilerCoverageSourceOption;

typedef struct {
  /**
   * Whether function coverage summaries should be displayed
   */
  bool functions;
  /**
   * Output CSV data of coverage
   */
  bool output_csv;
} InitiaCompilerCoverageSummaryOption;

typedef struct {
  uint8_t _private[0];
} db_t;

/**
 * A view into a `Option<&[u8]>`, created and maintained by Rust.
 *
 * This can be copied into a []byte in Go.
 */
typedef struct {
  /**
   * True if and only if this is None. If this is true, the other fields must be ignored.
   */
  bool is_none;
  const uint8_t *ptr;
  size_t len;
} U8SliceView;

typedef struct {
  /**
   * An ID assigned to this contract call
   */
  uint64_t call_id;
  uint64_t iterator_index;
} iterator_t;

typedef struct {
  int32_t (*next_db)(iterator_t, UnmanagedVector*, UnmanagedVector*);
} Iterator_vtable;

typedef struct {
  iterator_t state;
  Iterator_vtable vtable;
  size_t prefix_len;
} GoIter;

typedef struct {
  int32_t (*read_db)(db_t*, U8SliceView, UnmanagedVector*, UnmanagedVector*);
  int32_t (*write_db)(db_t*, U8SliceView, U8SliceView, UnmanagedVector*);
  int32_t (*remove_db)(db_t*, U8SliceView, UnmanagedVector*);
  int32_t (*scan_db)(db_t*,
                     U8SliceView,
                     U8SliceView,
                     U8SliceView,
                     int32_t,
                     GoIter*,
                     UnmanagedVector*);
} Db_vtable;

typedef struct {
  db_t *state;
  Db_vtable vtable;
} Db;

typedef struct {
  /**
   * Whether to include private declarations and implementations into the generated
   * documentation. Defaults to false.
   */
  bool include_impl;
  /**
   * Whether to include specifications in the generated documentation. Defaults to false.
   */
  bool include_specs;
  /**
   * Whether specifications should be put side-by-side with declarations or into a separate
   * section. Defaults to false.
   */
  bool specs_inlined;
  /**
   * Whether to include a dependency diagram. Defaults to false.
   */
  bool include_dep_diagram;
  /**
   * Whether details should be put into collapsed sections. This is not supported by
   * all markdown, but the github dialect. Defaults to false.
   */
  bool collapsed_sections;
  /**
   * Package-relative path to an optional markdown template which is a used to create a
   * landing page. Placeholders in this file are substituted as follows: `> {{move-toc}}` is
   * replaced by a table of contents of all modules; `> {{move-index}}` is replaced by an index,
   * and `> {{move-include NAME_OF_MODULE_OR_SCRIP}}` is replaced by the the full
   * documentation of the named entity. (The given entity will not longer be placed in
   * its own file, so this can be used to create a single manually populated page for
   * the package.)
   */
  ByteSliceView landing_page_template;
  /**
   * Package-relative path to a file whose content is added to each generated markdown file.
   * This can contain common markdown references fpr this package (e.g. `[move-book]: <url>`).
   */
  ByteSliceView references_file;
} InitiaCompilerDocgenOption;

typedef struct {
  uint8_t _private[0];
} api_t;

typedef struct {
  int32_t (*get_account_info)(const api_t*,
                              U8SliceView,
                              bool*,
                              uint64_t*,
                              uint64_t*,
                              uint8_t*,
                              UnmanagedVector*);
  int32_t (*amount_to_share)(const api_t*,
                             U8SliceView,
                             U8SliceView,
                             uint64_t,
                             uint64_t*,
                             UnmanagedVector*);
  int32_t (*share_to_amount)(const api_t*,
                             U8SliceView,
                             U8SliceView,
                             uint64_t,
                             uint64_t*,
                             UnmanagedVector*);
  int32_t (*unbond_timestamp)(const api_t*, uint64_t*, UnmanagedVector*);
} GoApi_vtable;

typedef struct {
  const api_t *state;
  GoApi_vtable vtable;
} GoApi;

typedef struct {
  ByteSliceView verbosity;
  /**
   * Filters targets out from the package. Any module with a matching file name will
   * be a target, similar as with `cargo test`.
   */
  ByteSliceView filter;
  /**
   * Whether to display additional information in error reports. This may help
   * debugging but also can make verification slower.
   */
  bool trace;
  /**
   * Whether to use cvc5 as the smt solver backend. The environment variable
   * `CVC5_EXE` should point to the binary.
   */
  bool cvc5;
  /**
   * The depth until which stratified functions are expanded.
   */
  size_t stratification_depth;
  /**
   * A seed for the prover.
   */
  size_t random_seed;
  /**
   * The number of cores to use for parallel processing of verification conditions.
   */
  size_t proc_cores;
  /**
   * A (soft) timeout for the solver, per verification condition, in seconds.
   */
  size_t vc_timeout;
  /**
   * Whether to check consistency of specs by injecting impossible assertions.
   */
  bool check_inconsistency;
  /**
   * Whether to keep loops as they are and pass them on to the underlying solver.
   */
  bool keep_loops;
  /**
   * Number of iterations to unroll loops. set 0 to unset
   */
  uint64_t loop_unroll;
  /**
   * Whether output for e.g. diagnosis shall be stable/redacted so it can be used in test
   * output.
   */
  bool stable_test_output;
  /**
   * Whether to dump intermediate step results to files.
   */
  bool dump;
  /**
   * indicating that this prover run is for a test.
   */
  bool for_test;
} InitiaCompilerProveOption;

typedef struct {
  /**
   * A filter string to determine which unit tests to run. A unit test will be run only if it
   * contains this string in its fully qualified (<addr>::<module_name>::<fn_name>) name.
   */
  ByteSliceView filter;
  /**
   * Report test statistics at the end of testing
   */
  bool report_statistics;
  /**
   * Show the storage state at the end of execution of a failing test
   */
  bool report_storage_on_error;
  /**
   * Ignore compiler's warning, and continue run tests
   */
  bool ignore_compile_warnings;
  /**
   * Collect coverage information for later use with the various `package coverage` subcommands
   */
  bool compute_coverage;
} InitiaCompilerTestOption;

vm_t *allocate_vm(size_t cache_capacity);

UnmanagedVector build_move_package(UnmanagedVector *errmsg, InitiaCompilerArgument initia_args);

UnmanagedVector clean_move_package(UnmanagedVector *errmsg,
                                   InitiaCompilerArgument initia_args,
                                   bool clean_cache,
                                   bool clean_byproduct,
                                   bool force);

UnmanagedVector convert_module_name(UnmanagedVector *errmsg,
                                    ByteSliceView precompiled,
                                    ByteSliceView module_name);

UnmanagedVector coverage_bytecode_move_package(UnmanagedVector *errmsg,
                                               InitiaCompilerArgument initia_args,
                                               InitiaCompilerCoverageBytecodeOption coverage_opt);

UnmanagedVector coverage_source_move_package(UnmanagedVector *errmsg,
                                             InitiaCompilerArgument initia_args,
                                             InitiaCompilerCoverageSourceOption coverage_opt);

UnmanagedVector coverage_summary_move_package(UnmanagedVector *errmsg,
                                              InitiaCompilerArgument initia_args,
                                              InitiaCompilerCoverageSummaryOption coverage_opt);

UnmanagedVector create_new_move_package(UnmanagedVector *errmsg,
                                        InitiaCompilerArgument initia_args,
                                        ByteSliceView name_view);

UnmanagedVector decode_module_bytes(UnmanagedVector *errmsg, ByteSliceView module_bytes);

UnmanagedVector decode_move_resource(Db db,
                                     UnmanagedVector *errmsg,
                                     ByteSliceView struct_tag,
                                     ByteSliceView resource_bytes);

UnmanagedVector decode_move_value(Db db,
                                  UnmanagedVector *errmsg,
                                  ByteSliceView type_tag,
                                  ByteSliceView value_bytes);

UnmanagedVector decode_script_bytes(UnmanagedVector *errmsg, ByteSliceView script_bytes);

void destroy_unmanaged_vector(UnmanagedVector v);

UnmanagedVector docgen_move_package(UnmanagedVector *errmsg,
                                    InitiaCompilerArgument initia_args,
                                    InitiaCompilerDocgenOption docgen_opt);

UnmanagedVector execute_contract(vm_t *vm_ptr,
                                 Db db,
                                 GoApi api,
                                 ByteSliceView env_payload,
                                 uint64_t gas_limit,
                                 ByteSliceView senders,
                                 ByteSliceView entry_function_payload,
                                 UnmanagedVector *errmsg);

UnmanagedVector execute_script(vm_t *vm_ptr,
                               Db db,
                               GoApi api,
                               ByteSliceView env_payload,
                               uint64_t gas_limit,
                               ByteSliceView senders,
                               ByteSliceView script_payload,
                               UnmanagedVector *errmsg);

UnmanagedVector execute_view_function(vm_t *vm_ptr,
                                      Db db,
                                      GoApi api,
                                      ByteSliceView env_payload,
                                      uint64_t gas_limit,
                                      ByteSliceView view_function_payload,
                                      UnmanagedVector *errmsg);

void initialize(vm_t *vm_ptr,
                Db db,
                GoApi api,
                ByteSliceView env_payload,
                ByteSliceView module_bundle_payload,
                bool allow_arbitrary,
                ByteSliceView allowed_publishers_payload,
                UnmanagedVector *errmsg);

UnmanagedVector new_unmanaged_vector(bool nil, const uint8_t *ptr, size_t length);

UnmanagedVector parse_struct_tag(UnmanagedVector *errmsg, ByteSliceView struct_tag_str);

UnmanagedVector prove_move_package(UnmanagedVector *errmsg,
                                   InitiaCompilerArgument initia_args,
                                   InitiaCompilerProveOption prove_opt);

UnmanagedVector read_module_info(UnmanagedVector *errmsg, ByteSliceView compiled);

void release_vm(vm_t *vm);

UnmanagedVector stringify_struct_tag(UnmanagedVector *errmsg, ByteSliceView struct_tag);

UnmanagedVector test_move_package(UnmanagedVector *errmsg,
                                  InitiaCompilerArgument initia_args,
                                  InitiaCompilerTestOption test_opt);

/**
 * Returns a version number of this library as a C string.
 *
 * The string is owned by libinitia and must not be mutated or destroyed by the caller.
 */
const char *version_str(void);

#endif /* __LIBINITIAVM__ */
