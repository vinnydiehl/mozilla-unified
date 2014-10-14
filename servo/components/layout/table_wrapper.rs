/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! CSS tables.
//!
//! This follows the "More Precise Definitions of Inline Layout and Table Layout" proposal written
//! by L. David Baron (Mozilla) here:
//!
//!     http://dbaron.org/css/intrinsic/
//!
//! Hereafter this document is referred to as INTRINSIC.

#![deny(unsafe_block)]

use block::{BlockFlow, BlockNonReplaced, FloatNonReplaced, ISizeAndMarginsComputer};
use block::{MarginsMayNotCollapse};
use construct::FlowConstructor;
use context::LayoutContext;
use floats::FloatKind;
use flow::{TableWrapperFlowClass, FlowClass, Flow, ImmutableFlowUtils};
use fragment::Fragment;
use table::ColumnInlineSize;
use wrapper::ThreadSafeLayoutNode;

use servo_util::geometry::Au;
use std::cmp::{max, min};
use std::fmt;
use style::CSSFloat;
use style::computed_values::{clear, float, table_layout};

#[deriving(Encodable)]
pub enum TableLayout {
    FixedLayout,
    AutoLayout
}

/// A table wrapper flow based on a block formatting context.
#[deriving(Encodable)]
pub struct TableWrapperFlow {
    pub block_flow: BlockFlow,

    /// Inline-size information for each column.
    pub column_inline_sizes: Vec<ColumnInlineSize>,

    /// Table-layout property
    pub table_layout: TableLayout,
}

impl TableWrapperFlow {
    pub fn from_node_and_fragment(node: &ThreadSafeLayoutNode,
                                  fragment: Fragment)
                                  -> TableWrapperFlow {
        let mut block_flow = BlockFlow::from_node_and_fragment(node, fragment);
        let table_layout = if block_flow.fragment().style().get_table().table_layout ==
                              table_layout::fixed {
            FixedLayout
        } else {
            AutoLayout
        };
        TableWrapperFlow {
            block_flow: block_flow,
            column_inline_sizes: vec!(),
            table_layout: table_layout
        }
    }

    pub fn from_node(constructor: &mut FlowConstructor,
                     node: &ThreadSafeLayoutNode)
                     -> TableWrapperFlow {
        let mut block_flow = BlockFlow::from_node(constructor, node);
        let table_layout = if block_flow.fragment().style().get_table().table_layout ==
                              table_layout::fixed {
            FixedLayout
        } else {
            AutoLayout
        };
        TableWrapperFlow {
            block_flow: block_flow,
            column_inline_sizes: vec!(),
            table_layout: table_layout
        }
    }

    pub fn float_from_node_and_fragment(node: &ThreadSafeLayoutNode,
                                        fragment: Fragment,
                                        float_kind: FloatKind)
                                        -> TableWrapperFlow {
        let mut block_flow = BlockFlow::float_from_node_and_fragment(node, fragment, float_kind);
        let table_layout = if block_flow.fragment().style().get_table().table_layout ==
                              table_layout::fixed {
            FixedLayout
        } else {
            AutoLayout
        };
        TableWrapperFlow {
            block_flow: block_flow,
            column_inline_sizes: vec!(),
            table_layout: table_layout
        }
    }

    pub fn build_display_list_table_wrapper(&mut self, layout_context: &LayoutContext) {
        debug!("build_display_list_table_wrapper: same process as block flow");
        self.block_flow.build_display_list_block(layout_context);
    }

    /// Calculates table column sizes for automatic layout per INTRINSIC § 4.3.
    fn calculate_table_column_sizes_for_automatic_layout(&mut self) {
        // Find the padding and border of our first child, which is the table itself.
        //
        // This is a little weird because we're computing border/padding/margins for our child,
        // when normally the child computes it itself. But it has to be this way because the
        // padding will affect where we place the child. This is an odd artifact of the way that
        // tables are separated into table flows and table wrapper flows.
        let available_inline_size = self.block_flow.fragment.border_box.size.inline;
        let mut table_border_padding = Au(0);
        for kid in self.block_flow.base.child_iter() {
            if kid.is_table() {
                let kid_block = kid.as_block();
                kid_block.fragment.compute_border_and_padding(available_inline_size);
                kid_block.fragment.compute_block_direction_margins(available_inline_size);
                kid_block.fragment.compute_inline_direction_margins(available_inline_size);
                table_border_padding = kid_block.fragment.border_padding.inline_start_end();
                break
            }
        }

        // FIXME(pcwalton, spec): INTRINSIC § 8 does not properly define how to compute this, but
        // says "the basic idea is the same as the shrink-to-fit width that CSS2.1 defines". So we
        // just use the shrink-to-fit inline size.
        let available_inline_size =
            self.block_flow.get_shrink_to_fit_inline_size(available_inline_size);

        // Compute all the guesses for the column sizes, and sum them.
        let mut total_guess = AutoLayoutCandidateGuess::new();
        let guesses: Vec<AutoLayoutCandidateGuess> =
            self.column_inline_sizes.iter().map(|column_inline_size| {
                let guess = AutoLayoutCandidateGuess::from_column_inline_size(
                    column_inline_size,
                    available_inline_size);
                total_guess = total_guess + guess;
                guess
            }).collect();

        // Assign inline sizes.
        let selection = SelectedAutoLayoutCandidateGuess::select(&total_guess,
                                                                 available_inline_size);
        let mut total_used_inline_size = Au(0);
        for (column_inline_size, guess) in self.column_inline_sizes
                                               .iter_mut()
                                               .zip(guesses.iter()) {
            column_inline_size.minimum_length = guess.calculate(selection);
            column_inline_size.percentage = 0.0;
            total_used_inline_size = total_used_inline_size + column_inline_size.minimum_length
        }

        // Distribute excess inline-size if necessary per INTRINSIC § 4.4.
        //
        // FIXME(pcwalton, spec): How do I deal with fractional excess?
        let excess_inline_size = available_inline_size - total_used_inline_size;
        if excess_inline_size > Au(0) &&
                selection == UsePreferredGuessAndDistributeExcessInlineSize {
            let mut info = ExcessInlineSizeDistributionInfo::new();
            for column_inline_size in self.column_inline_sizes.iter() {
                info.update(column_inline_size)
            }

            let mut total_distributed_excess_size = Au(0);
            for column_inline_size in self.column_inline_sizes.iter_mut() {
                info.distribute_excess_inline_size_to_column(column_inline_size,
                                                             excess_inline_size,
                                                             &mut total_distributed_excess_size)
            }
            total_used_inline_size = available_inline_size
        }

        self.block_flow.fragment.border_box.size.inline = total_used_inline_size +
            table_border_padding;
        self.block_flow.base.position.size.inline = total_used_inline_size +
            table_border_padding + self.block_flow.fragment.margin.inline_start_end();
    }

    fn compute_used_inline_size(&mut self,
                                layout_context: &LayoutContext,
                                parent_flow_inline_size: Au) {
        // Delegate to the appropriate inline size computer to find the constraint inputs.
        let input = if self.is_float() {
            FloatNonReplaced.compute_inline_size_constraint_inputs(&mut self.block_flow,
                                                                   parent_flow_inline_size,
                                                                   layout_context)
        } else {
            BlockNonReplaced.compute_inline_size_constraint_inputs(&mut self.block_flow,
                                                                   parent_flow_inline_size,
                                                                   layout_context)
        };

        // Delegate to the appropriate inline size computer to write the constraint solutions in.
        if self.is_float() {
            let solution = FloatNonReplaced.solve_inline_size_constraints(&mut self.block_flow,
                                                                          &input);
            FloatNonReplaced.set_inline_size_constraint_solutions(&mut self.block_flow, solution);
            FloatNonReplaced.set_flow_x_coord_if_necessary(&mut self.block_flow, solution);
        } else {
            let solution = BlockNonReplaced.solve_inline_size_constraints(&mut self.block_flow,
                                                                          &input);
            BlockNonReplaced.set_inline_size_constraint_solutions(&mut self.block_flow, solution);
            BlockNonReplaced.set_flow_x_coord_if_necessary(&mut self.block_flow, solution);
        }
    }
}

impl Flow for TableWrapperFlow {
    fn class(&self) -> FlowClass {
        TableWrapperFlowClass
    }

    fn is_float(&self) -> bool {
        self.block_flow.is_float()
    }

    fn as_table_wrapper<'a>(&'a mut self) -> &'a mut TableWrapperFlow {
        self
    }

    fn as_immutable_table_wrapper<'a>(&'a self) -> &'a TableWrapperFlow {
        self
    }

    fn as_block<'a>(&'a mut self) -> &'a mut BlockFlow {
        &mut self.block_flow
    }

    fn float_clearance(&self) -> clear::T {
        self.block_flow.float_clearance()
    }

    fn float_kind(&self) -> float::T {
        self.block_flow.float_kind()
    }

    fn bubble_inline_sizes(&mut self) {
        // Get the column inline-sizes info from the table flow.
        for kid in self.block_flow.base.child_iter() {
            debug_assert!(kid.is_table_caption() || kid.is_table());
            if kid.is_table() {
                self.column_inline_sizes = kid.column_inline_sizes().clone()
            }
        }

        self.block_flow.bubble_inline_sizes();
    }

    fn assign_inline_sizes(&mut self, layout_context: &LayoutContext) {
        debug!("assign_inline_sizes({}): assigning inline_size for flow",
               if self.is_float() {
                   "floated table_wrapper"
               } else {
                   "table_wrapper"
               });

        // Table wrappers are essentially block formatting contexts and are therefore never
        // impacted by floats.
        self.block_flow.base.flags.set_impacted_by_left_floats(false);
        self.block_flow.base.flags.set_impacted_by_right_floats(false);

        // Our inline-size was set to the inline-size of the containing block by the flow's parent.
        // Now compute the real value.
        let containing_block_inline_size = self.block_flow.base.block_container_inline_size;
        if self.is_float() {
            self.block_flow.float.as_mut().unwrap().containing_inline_size =
                containing_block_inline_size;
        }

        self.compute_used_inline_size(layout_context, containing_block_inline_size);

        match self.table_layout {
            FixedLayout => {}
            AutoLayout => {
                self.calculate_table_column_sizes_for_automatic_layout()
            }
        }

        let inline_start_content_edge = self.block_flow.fragment.border_box.start.i;
        let content_inline_size = self.block_flow.fragment.border_box.size.inline;

        // In case of fixed layout, column inline-sizes are calculated in table flow.
        let assigned_column_inline_sizes = match self.table_layout {
            FixedLayout => None,
            AutoLayout => Some(self.column_inline_sizes.as_slice())
        };

        self.block_flow.propagate_assigned_inline_size_to_children(inline_start_content_edge,
                                                                   content_inline_size,
                                                                   assigned_column_inline_sizes);
    }

    fn assign_block_size<'a>(&mut self, ctx: &'a LayoutContext<'a>) {
        debug!("assign_block_size: assigning block_size for table_wrapper");
        self.block_flow.assign_block_size_block_base(ctx, MarginsMayNotCollapse);
    }

    fn compute_absolute_position(&mut self) {
        self.block_flow.compute_absolute_position()
    }

    fn assign_block_size_for_inorder_child_if_necessary<'a>(&mut self,
                                                            layout_context: &'a LayoutContext<'a>)
                                                            -> bool {
        if self.block_flow.is_float() {
            self.block_flow.place_float();
            return true
        }

        let impacted = self.block_flow.base.flags.impacted_by_floats();
        if impacted {
            self.assign_block_size(layout_context);
        }
        impacted
    }

    fn update_late_computed_inline_position_if_necessary(&mut self, inline_position: Au) {
        self.block_flow.update_late_computed_inline_position_if_necessary(inline_position)
    }

    fn update_late_computed_block_position_if_necessary(&mut self, block_position: Au) {
        self.block_flow.update_late_computed_block_position_if_necessary(block_position)
    }
}

impl fmt::Show for TableWrapperFlow {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        if self.is_float() {
            write!(f, "TableWrapperFlow(Float): {}", self.block_flow.fragment)
        } else {
            write!(f, "TableWrapperFlow: {}", self.block_flow.fragment)
        }
    }
}

/// The layout "guesses" defined in INTRINSIC § 4.3.
struct AutoLayoutCandidateGuess {
    /// The column inline-size assignment where each column is assigned its intrinsic minimum
    /// inline-size.
    minimum_guess: Au,

    /// The column inline-size assignment where:
    ///   * A column with an intrinsic percentage inline-size greater than 0% is assigned the
    ///     larger of:
    ///     - Its intrinsic percentage inline-size times the assignable inline-size;
    ///     - Its intrinsic minimum inline-size;
    ///   * Other columns receive their intrinsic minimum inline-size.
    minimum_percentage_guess: Au,

    /// The column inline-size assignment where:
    ///   * Each column with an intrinsic percentage inline-size greater than 0% is assigned the
    ///     larger of:
    ///     - Its intrinsic percentage inline-size times the assignable inline-size;
    ///     - Its intrinsic minimum inline-size;
    ///   * Any other column that is constrained is assigned its intrinsic preferred inline-size;
    ///   * Other columns are assigned their intrinsic minimum inline-size.
    minimum_specified_guess: Au,

    /// The column inline-size assignment where:
    ///   * Each column with an intrinsic percentage inline-size greater than 0% is assigned the
    ///     larger of:
    ///     - Its intrinsic percentage inline-size times the assignable inline-size;
    ///     - Its intrinsic minimum inline-size;
    ///   * Other columns are assigned their intrinsic preferred inline-size.
    preferred_guess: Au,
}

impl AutoLayoutCandidateGuess {
    /// Creates a guess with all elements initialized to zero.
    fn new() -> AutoLayoutCandidateGuess {
        AutoLayoutCandidateGuess {
            minimum_guess: Au(0),
            minimum_percentage_guess: Au(0),
            minimum_specified_guess: Au(0),
            preferred_guess: Au(0),
        }
    }

    /// Fills in the inline-size guesses for this column per INTRINSIC § 4.3.
    fn from_column_inline_size(column_inline_size: &ColumnInlineSize, assignable_inline_size: Au)
                               -> AutoLayoutCandidateGuess {
        let minimum_percentage_guess =
            max(assignable_inline_size.scale_by(column_inline_size.percentage),
                column_inline_size.minimum_length);
        AutoLayoutCandidateGuess {
            minimum_guess: column_inline_size.minimum_length,
            minimum_percentage_guess: minimum_percentage_guess,
            // FIXME(pcwalton): We need the notion of *constrainedness* per INTRINSIC § 4 to
            // implement this one correctly.
            minimum_specified_guess: if column_inline_size.percentage > 0.0 {
                minimum_percentage_guess
            } else if column_inline_size.constrained {
                column_inline_size.preferred
            } else {
                column_inline_size.minimum_length
            },
            preferred_guess: if column_inline_size.percentage > 0.0 {
                minimum_percentage_guess
            } else {
                column_inline_size.preferred
            },
        }
    }

    /// Calculates the inline-size, interpolating appropriately based on the value of `selection`.
    ///
    /// This does *not* distribute excess inline-size. That must be done later if necessary.
    fn calculate(&self, selection: SelectedAutoLayoutCandidateGuess) -> Au {
        match selection {
            UseMinimumGuess => self.minimum_guess,
            InterpolateBetweenMinimumGuessAndMinimumPercentageGuess(weight) => {
                interp(self.minimum_guess, self.minimum_percentage_guess, weight)
            }
            InterpolateBetweenMinimumPercentageGuessAndMinimumSpecifiedGuess(weight) => {
                interp(self.minimum_percentage_guess, self.minimum_specified_guess, weight)
            }
            InterpolateBetweenMinimumSpecifiedGuessAndPreferredGuess(weight) => {
                interp(self.minimum_specified_guess, self.preferred_guess, weight)
            }
            UsePreferredGuessAndDistributeExcessInlineSize => {
                self.preferred_guess
            }
        }
    }
}

impl Add<AutoLayoutCandidateGuess,AutoLayoutCandidateGuess> for AutoLayoutCandidateGuess {
    #[inline]
    fn add(&self, other: &AutoLayoutCandidateGuess) -> AutoLayoutCandidateGuess {
        AutoLayoutCandidateGuess {
            minimum_guess: self.minimum_guess + other.minimum_guess,
            minimum_percentage_guess:
                self.minimum_percentage_guess + other.minimum_percentage_guess,
            minimum_specified_guess: self.minimum_specified_guess + other.minimum_specified_guess,
            preferred_guess: self.preferred_guess + other.preferred_guess,
        }
    }
}

/// The `CSSFloat` member specifies the weight of the smaller of the two guesses, on a scale from
/// 0.0 to 1.0.
#[deriving(PartialEq, Show)]
enum SelectedAutoLayoutCandidateGuess {
    UseMinimumGuess,
    InterpolateBetweenMinimumGuessAndMinimumPercentageGuess(CSSFloat),
    InterpolateBetweenMinimumPercentageGuessAndMinimumSpecifiedGuess(CSSFloat),
    InterpolateBetweenMinimumSpecifiedGuessAndPreferredGuess(CSSFloat),
    UsePreferredGuessAndDistributeExcessInlineSize,
}

impl SelectedAutoLayoutCandidateGuess {
    /// See INTRINSIC § 4.3.
    ///
    /// FIXME(pcwalton, INTRINSIC spec): INTRINSIC doesn't specify whether these are exclusive or
    /// inclusive ranges.
    fn select(guess: &AutoLayoutCandidateGuess, assignable_inline_size: Au)
              -> SelectedAutoLayoutCandidateGuess {
        if assignable_inline_size < guess.minimum_guess {
            UseMinimumGuess
        } else if assignable_inline_size < guess.minimum_percentage_guess {
            let weight = weight(guess.minimum_guess,
                                assignable_inline_size,
                                guess.minimum_percentage_guess);
            InterpolateBetweenMinimumGuessAndMinimumPercentageGuess(weight)
        } else if assignable_inline_size < guess.minimum_specified_guess {
            let weight = weight(guess.minimum_percentage_guess,
                                assignable_inline_size,
                                guess.minimum_specified_guess);
            InterpolateBetweenMinimumPercentageGuessAndMinimumSpecifiedGuess(weight)
        } else if assignable_inline_size < guess.preferred_guess {
            let weight = weight(guess.minimum_specified_guess,
                                assignable_inline_size,
                                guess.preferred_guess);
            InterpolateBetweenMinimumSpecifiedGuessAndPreferredGuess(weight)
        } else {
            UsePreferredGuessAndDistributeExcessInlineSize
        }
    }
}

/// Computes the weight needed to linearly interpolate `middle` between two guesses `low` and
/// `high` as specified by INTRINSIC § 4.3.
fn weight(low: Au, middle: Au, high: Au) -> CSSFloat {
    (middle - low).to_subpx() / (high - low).to_subpx()
}

/// Linearly interpolates between two guesses, as specified by INTRINSIC § 4.3.
fn interp(low: Au, high: Au, weight: CSSFloat) -> Au {
    low + (high - low).scale_by(weight)
}

struct ExcessInlineSizeDistributionInfo {
    preferred_inline_size_of_nonconstrained_columns_with_no_percentage: Au,
    count_of_nonconstrained_columns_with_no_percentage: u32,
    preferred_inline_size_of_constrained_columns_with_no_percentage: Au,
    total_percentage: CSSFloat,
    column_count: u32,
}

impl ExcessInlineSizeDistributionInfo {
    fn new() -> ExcessInlineSizeDistributionInfo {
        ExcessInlineSizeDistributionInfo {
            preferred_inline_size_of_nonconstrained_columns_with_no_percentage: Au(0),
            count_of_nonconstrained_columns_with_no_percentage: 0,
            preferred_inline_size_of_constrained_columns_with_no_percentage: Au(0),
            total_percentage: 0.0,
            column_count: 0,
        }
    }

    fn update(&mut self, column_inline_size: &ColumnInlineSize) {
        if !column_inline_size.constrained && column_inline_size.percentage == 0.0 {
            self.preferred_inline_size_of_nonconstrained_columns_with_no_percentage =
                self.preferred_inline_size_of_nonconstrained_columns_with_no_percentage +
                column_inline_size.preferred;
            self.count_of_nonconstrained_columns_with_no_percentage += 1
        }
        if column_inline_size.constrained && column_inline_size.percentage == 0.0 {
            self.preferred_inline_size_of_constrained_columns_with_no_percentage =
                self.preferred_inline_size_of_constrained_columns_with_no_percentage +
                column_inline_size.preferred
        }
        self.total_percentage += column_inline_size.percentage;
        self.column_count += 1
    }

    /// Based on the information here, distributes excess inline-size to the given column per
    /// INTRINSIC § 4.4.
    ///
    /// `#[inline]` so the compiler will hoist out the branch, which is loop-invariant.
    #[inline]
    fn distribute_excess_inline_size_to_column(&self,
                                               column_inline_size: &mut ColumnInlineSize,
                                               excess_inline_size: Au,
                                               total_distributed_excess_size: &mut Au) {
        let proportion =
            if self.preferred_inline_size_of_nonconstrained_columns_with_no_percentage > Au(0) {
                column_inline_size.preferred.to_subpx() /
                    self.preferred_inline_size_of_nonconstrained_columns_with_no_percentage
                        .to_subpx()
            } else if self.count_of_nonconstrained_columns_with_no_percentage > 0 {
                1.0 / (self.count_of_nonconstrained_columns_with_no_percentage as CSSFloat)
            } else if self.preferred_inline_size_of_constrained_columns_with_no_percentage >
                    Au(0) {
                column_inline_size.preferred.to_subpx() /
                    self.preferred_inline_size_of_constrained_columns_with_no_percentage.to_subpx()
            } else if self.total_percentage > 0.0 {
                column_inline_size.percentage / self.total_percentage
            } else {
                1.0 / (self.column_count as CSSFloat)
            };

        // The `min` here has the effect of throwing away fractional excess at the end of the
        // table.
        let amount_to_distribute = min(excess_inline_size.scale_by(proportion),
                                       excess_inline_size - *total_distributed_excess_size);
        *total_distributed_excess_size = *total_distributed_excess_size + amount_to_distribute;
        column_inline_size.minimum_length = column_inline_size.minimum_length +
            amount_to_distribute
    }
}

