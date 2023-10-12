use std::collections::BTreeMap;

use crate::algebra::{AbstractValueSize, AbstractValueSizePerArg};
use crate::meter::{FromOnChainGasSchedule, InitialGasSchedule, ToOnChainGasSchedule};
use move_core_types::u256::U256;
use move_core_types::{account_address::AccountAddress, gas_algebra::NumArgs};
use move_vm_types::views::{ValueView, ValueVisitor};

crate::params::define_gas_parameters!(
    AbstractValueSizeGasParameters,
    "misc.abs_val",
    [
        // abstract value size
        [u8: AbstractValueSize, "u8", 40],
        [u16: AbstractValueSize, "u16", 40],
        [u32: AbstractValueSize, "u32", 40],
        [u64: AbstractValueSize, "u64", 40],
        [u128: AbstractValueSize, "u128", 40],
        [u256: AbstractValueSize, "u256", 40],
        [bool: AbstractValueSize, "bool", 40],
        [address: AbstractValueSize, "address", 40],
        [struct_: AbstractValueSize, "struct", 40],
        [vector: AbstractValueSize, "vector", 40],
        [reference: AbstractValueSize, "reference", 40],
        [per_u8_packed: AbstractValueSizePerArg, "per_u8_packed", 1],
        [per_u16_packed: AbstractValueSizePerArg, "per_u16_packed", 2],
        [per_u32_packed: AbstractValueSizePerArg, "per_u32_packed", 4],
        [per_u64_packed: AbstractValueSizePerArg, "per_u64_packed", 8],
        [
            per_u128_packed: AbstractValueSizePerArg,
            "per_u128_packed",
            16
        ],
        [
            per_u256_packed: AbstractValueSizePerArg,
            "per_u256_packed",
            32
        ],
        [
            per_bool_packed: AbstractValueSizePerArg,
            "per_bool_packed",
            1
        ],
        [
            per_address_packed: AbstractValueSizePerArg,
            "per_address_packed",
            32
        ],
    ]
);

struct DerefVisitor<V> {
    inner: V,
    offset: usize,
}

impl<V> DerefVisitor<V>
where
    V: ValueVisitor,
{
    pub fn new(visitor: V) -> Self {
        Self {
            inner: visitor,
            offset: 0,
        }
    }

    pub fn into_inner(self) -> V {
        self.inner
    }
}

macro_rules! deref_visitor_delegate_simple {
    ($([$fn: ident, $ty: ty $(,)?]),+ $(,)?) => {
        $(
            #[inline]
            fn $fn(&mut self, depth: usize, val: $ty) {
                self.inner.$fn(depth - self.offset, val);
            }
        )*
    };
}

impl<V> ValueVisitor for DerefVisitor<V>
where
    V: ValueVisitor,
{
    deref_visitor_delegate_simple!(
        [visit_u8, u8],
        [visit_u16, u16],
        [visit_u32, u32],
        [visit_u64, u64],
        [visit_u128, u128],
        [visit_u256, U256],
        [visit_bool, bool],
        [visit_address, AccountAddress],
        [visit_vec_u8, &[u8]],
        [visit_vec_u64, &[u64]],
        [visit_vec_u128, &[u128]],
        [visit_vec_bool, &[bool]],
        [visit_vec_address, &[AccountAddress]],
    );

    #[inline]
    fn visit_struct(&mut self, depth: usize, len: usize) -> bool {
        self.inner.visit_struct(depth - self.offset, len)
    }

    #[inline]
    fn visit_vec(&mut self, depth: usize, len: usize) -> bool {
        self.inner.visit_vec(depth - self.offset, len)
    }

    #[inline]
    fn visit_ref(&mut self, depth: usize, _is_global: bool) -> bool {
        assert_eq!(depth, 0, "There shouldn't be inner refs");
        self.offset = 1;
        true
    }
}

struct AbstractValueSizeVisitor<'a> {
    params: &'a AbstractValueSizeGasParameters,
    size: AbstractValueSize,
}

impl<'a> AbstractValueSizeVisitor<'a> {
    fn new(params: &'a AbstractValueSizeGasParameters) -> Self {
        Self {
            params,
            size: 0.into(),
        }
    }

    fn finish(self) -> AbstractValueSize {
        self.size
    }
}

impl<'a> ValueVisitor for AbstractValueSizeVisitor<'a> {
    #[inline]
    fn visit_u8(&mut self, _depth: usize, _val: u8) {
        self.size += self.params.u8;
    }

    #[inline]
    fn visit_u16(&mut self, _depth: usize, _val: u16) {
        self.size += self.params.u16;
    }

    #[inline]
    fn visit_u32(&mut self, _depth: usize, _val: u32) {
        self.size += self.params.u32;
    }

    #[inline]
    fn visit_u64(&mut self, _depth: usize, _val: u64) {
        self.size += self.params.u64;
    }

    #[inline]
    fn visit_u128(&mut self, _depth: usize, _val: u128) {
        self.size += self.params.u128;
    }

    #[inline]
    fn visit_bool(&mut self, _depth: usize, _val: bool) {
        self.size += self.params.bool;
    }

    #[inline]
    fn visit_u256(&mut self, _depth: usize, _val: U256) {
        self.size += self.params.u256;
    }

    #[inline]
    fn visit_address(&mut self, _depth: usize, _val: AccountAddress) {
        self.size += self.params.address;
    }

    #[inline]
    fn visit_struct(&mut self, _depth: usize, _len: usize) -> bool {
        self.size += self.params.struct_;
        true
    }

    #[inline]
    fn visit_vec(&mut self, _depth: usize, _len: usize) -> bool {
        self.size += self.params.vector;
        true
    }

    #[inline]
    fn visit_vec_u8(&mut self, _depth: usize, vals: &[u8]) {
        let mut size = self.params.per_u8_packed * NumArgs::new(vals.len() as u64);
        size += self.params.vector;
        self.size += size;
    }

    #[inline]
    fn visit_vec_u64(&mut self, _depth: usize, vals: &[u64]) {
        let mut size = self.params.per_u64_packed * NumArgs::new(vals.len() as u64);
        size += self.params.vector;
        self.size += size;
    }

    #[inline]
    fn visit_vec_u128(&mut self, _depth: usize, vals: &[u128]) {
        let mut size = self.params.per_u128_packed * NumArgs::new(vals.len() as u64);
        size += self.params.vector;
        self.size += size;
    }

    #[inline]
    fn visit_vec_bool(&mut self, _depth: usize, vals: &[bool]) {
        let mut size = self.params.per_bool_packed * NumArgs::new(vals.len() as u64);
        size += self.params.vector;
        self.size += size;
    }

    #[inline]
    fn visit_vec_address(&mut self, _depth: usize, vals: &[AccountAddress]) {
        let mut size = self.params.per_address_packed * NumArgs::new(vals.len() as u64);
        size += self.params.vector;
        self.size += size;
    }

    #[inline]
    fn visit_ref(&mut self, _depth: usize, _is_global: bool) -> bool {
        self.size += self.params.reference;
        false
    }
}

impl AbstractValueSizeGasParameters {
    /// Calculates the abstract size of the given value.
    pub fn abstract_value_size(&self, val: impl ValueView) -> AbstractValueSize {
        let mut visitor = AbstractValueSizeVisitor::new(self);
        val.visit(&mut visitor);
        visitor.finish()
    }

    /// Calculates the abstract size of the given value.
    /// If the value is a reference, then the size of the value behind it will be returned.
    pub fn abstract_value_size_dereferenced(&self, val: impl ValueView) -> AbstractValueSize {
        let mut visitor = DerefVisitor::new(AbstractValueSizeVisitor::new(self));
        val.visit(&mut visitor);
        visitor.into_inner().finish()
    }
}

impl AbstractValueSizeGasParameters {
    pub fn abstract_stack_size(&self, val: impl ValueView) -> AbstractValueSize {
        struct Visitor<'a> {
            params: &'a AbstractValueSizeGasParameters,
            res: Option<AbstractValueSize>,
        }

        impl<'a> ValueVisitor for Visitor<'a> {
            #[inline]
            fn visit_u8(&mut self, _depth: usize, _val: u8) {
                self.res = Some(self.params.u8);
            }

            #[inline]
            fn visit_u16(&mut self, _depth: usize, _val: u16) {
                self.res = Some(self.params.u16);
            }

            #[inline]
            fn visit_u32(&mut self, _depth: usize, _val: u32) {
                self.res = Some(self.params.u32);
            }

            #[inline]
            fn visit_u64(&mut self, _depth: usize, _val: u64) {
                self.res = Some(self.params.u64);
            }

            #[inline]
            fn visit_u128(&mut self, _depth: usize, _val: u128) {
                self.res = Some(self.params.u128);
            }

            #[inline]
            fn visit_u256(&mut self, _depth: usize, _val: U256) {
                self.res = Some(self.params.u256);
            }

            #[inline]
            fn visit_bool(&mut self, _depth: usize, _val: bool) {
                self.res = Some(self.params.bool);
            }

            #[inline]
            fn visit_address(&mut self, _depth: usize, _val: AccountAddress) {
                self.res = Some(self.params.address);
            }

            #[inline]
            fn visit_struct(&mut self, _depth: usize, _len: usize) -> bool {
                self.res = Some(self.params.struct_);
                false
            }

            #[inline]
            fn visit_vec(&mut self, _depth: usize, _len: usize) -> bool {
                self.res = Some(self.params.vector);
                false
            }

            #[inline]
            fn visit_ref(&mut self, _depth: usize, _is_global: bool) -> bool {
                self.res = Some(self.params.reference);
                false
            }

            #[inline]
            fn visit_vec_u8(&mut self, depth: usize, vals: &[u8]) {
                self.visit_vec(depth, vals.len());
            }

            #[inline]
            fn visit_vec_u64(&mut self, depth: usize, vals: &[u64]) {
                self.visit_vec(depth, vals.len());
            }

            #[inline]
            fn visit_vec_u128(&mut self, depth: usize, vals: &[u128]) {
                self.visit_vec(depth, vals.len());
            }

            #[inline]
            fn visit_vec_bool(&mut self, depth: usize, vals: &[bool]) {
                self.visit_vec(depth, vals.len());
            }

            #[inline]
            fn visit_vec_address(&mut self, depth: usize, vals: &[AccountAddress]) {
                self.visit_vec(depth, vals.len());
            }
        }

        let mut visitor = Visitor {
            params: self,
            res: None,
        };
        val.visit(&mut visitor);
        visitor.res.unwrap()
    }

    pub fn abstract_packed_size(&self, val: impl ValueView) -> AbstractValueSize {
        struct Visitor<'a> {
            params: &'a AbstractValueSizeGasParameters,
            res: Option<AbstractValueSize>,
        }

        impl<'a> ValueVisitor for Visitor<'a> {
            #[inline]
            fn visit_u8(&mut self, _depth: usize, _val: u8) {
                self.res = Some(self.params.per_u8_packed * NumArgs::from(1));
            }

            #[inline]
            fn visit_u16(&mut self, _depth: usize, _val: u16) {
                self.res = Some(self.params.per_u16_packed * NumArgs::from(1));
            }

            #[inline]
            fn visit_u32(&mut self, _depth: usize, _val: u32) {
                self.res = Some(self.params.per_u32_packed * NumArgs::from(1));
            }

            #[inline]
            fn visit_u64(&mut self, _depth: usize, _val: u64) {
                self.res = Some(self.params.per_u64_packed * NumArgs::from(1));
            }

            #[inline]
            fn visit_u128(&mut self, _depth: usize, _val: u128) {
                self.res = Some(self.params.per_u128_packed * NumArgs::from(1));
            }

            #[inline]
            fn visit_u256(&mut self, _depth: usize, _val: U256) {
                self.res = Some(self.params.per_u256_packed * NumArgs::from(1));
            }

            #[inline]
            fn visit_bool(&mut self, _depth: usize, _val: bool) {
                self.res = Some(self.params.per_bool_packed * NumArgs::from(1));
            }

            #[inline]
            fn visit_address(&mut self, _depth: usize, _val: AccountAddress) {
                self.res = Some(self.params.per_address_packed * NumArgs::from(1));
            }

            #[inline]
            fn visit_struct(&mut self, _depth: usize, _len: usize) -> bool {
                self.res = Some(self.params.struct_);
                false
            }

            #[inline]
            fn visit_vec(&mut self, _depth: usize, _len: usize) -> bool {
                self.res = Some(self.params.vector);
                false
            }

            #[inline]
            fn visit_ref(&mut self, _depth: usize, _is_global: bool) -> bool {
                // TODO(Gas): This should be unreachable...
                //            See if we can handle this in a more graceful way.
                self.res = Some(self.params.reference);
                false
            }

            // TODO(Gas): The following function impls are necessary due to a bug upstream.
            //            Remove them once the bug is fixed.
            #[inline]
            fn visit_vec_u8(&mut self, depth: usize, vals: &[u8]) {
                self.visit_vec(depth, vals.len());
            }

            #[inline]
            fn visit_vec_u64(&mut self, depth: usize, vals: &[u64]) {
                self.visit_vec(depth, vals.len());
            }

            #[inline]
            fn visit_vec_u128(&mut self, depth: usize, vals: &[u128]) {
                self.visit_vec(depth, vals.len());
            }

            #[inline]
            fn visit_vec_bool(&mut self, depth: usize, vals: &[bool]) {
                self.visit_vec(depth, vals.len());
            }

            #[inline]
            fn visit_vec_address(&mut self, depth: usize, vals: &[AccountAddress]) {
                self.visit_vec(depth, vals.len());
            }
        }

        let mut visitor = Visitor {
            params: self,
            res: None,
        };
        val.visit(&mut visitor);
        visitor.res.unwrap()
    }

    pub fn abstract_value_size_stack_and_heap(
        &self,
        val: impl ValueView,
    ) -> (AbstractValueSize, AbstractValueSize) {
        let stack_size = self.abstract_stack_size(&val);
        let abs_size = self.abstract_value_size(val);
        let heap_size = abs_size.checked_sub(stack_size).unwrap_or_else(|| 0.into());

        (stack_size, heap_size)
    }

    pub fn abstract_heap_size(&self, val: impl ValueView) -> AbstractValueSize {
        let stack_size = self.abstract_stack_size(&val);
        let abs_size = self.abstract_value_size(val);

        abs_size.checked_sub(stack_size).unwrap_or_else(|| 0.into())
    }
}

/// Miscellaneous gas parameters.
#[derive(Debug, Clone)]
pub struct MiscGasParameters {
    pub abs_val: AbstractValueSizeGasParameters,
}

impl FromOnChainGasSchedule for MiscGasParameters {
    fn from_on_chain_gas_schedule(gas_schedule: &BTreeMap<String, u64>) -> Option<Self> {
        Some(Self {
            abs_val: FromOnChainGasSchedule::from_on_chain_gas_schedule(gas_schedule)?,
        })
    }
}

impl ToOnChainGasSchedule for MiscGasParameters {
    fn to_on_chain_gas_schedule(&self) -> Vec<(String, u64)> {
        self.abs_val.to_on_chain_gas_schedule()
    }
}

impl MiscGasParameters {
    pub fn zeros() -> Self {
        Self {
            abs_val: AbstractValueSizeGasParameters::zeros(),
        }
    }
}

impl InitialGasSchedule for MiscGasParameters {
    fn initial() -> Self {
        Self {
            abs_val: InitialGasSchedule::initial(),
        }
    }
}
