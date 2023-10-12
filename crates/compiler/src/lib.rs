#![cfg_attr(feature = "backtraces", feature(backtrace))]
#![allow(clippy::not_unsafe_ptr_arg_deref, clippy::missing_safety_doc)]

pub mod built_package;
pub mod clean;
pub mod command;
pub mod compiler;
pub mod docgen;
pub mod extended_checks;
pub mod new;
pub mod prover;

pub mod extensions;
mod mocks;

pub use clean::Clean;
pub use command::Command;
pub use compiler::compile;
pub use new::New;

#[cfg(test)]
mod tests;
