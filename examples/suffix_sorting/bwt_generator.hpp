/*******************************************************************************
 * examples/suffix_sorting/bwt_generator.hpp
 *
 * Part of Project Thrill - http://project-thrill.org
 *
 * Copyright (C) 2016 Florian Kurpicz <florian.kurpicz@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-2 license in the LICENSE file.
 ******************************************************************************/

#pragma once
#ifndef THRILL_EXAMPLES_SUFFIX_SORTING_BWT_GENERATOR_HEADER
#define THRILL_EXAMPLES_SUFFIX_SORTING_BWT_GENERATOR_HEADER

#include <thrill/api/collapse.hpp>
#include <thrill/api/generate.hpp>
#include <thrill/api/max.hpp>
#include <thrill/api/size.hpp>
#include <thrill/api/sort.hpp>
#include <thrill/api/window.hpp>
#include <thrill/api/zip.hpp>

namespace examples {
namespace suffix_sorting {

template <typename InputDIA, typename SuffixArrayDIA>
InputDIA GenerateBWT(const InputDIA& input, const SuffixArrayDIA& suffix_array) {

    using namespace thrill; // NOLINT

    Context& ctx = input.ctx();

    using Char = typename InputDIA::ValueType;
    using Index = typename SuffixArrayDIA::ValueType;

    struct IndexRank {
        Index index;
        Index rank;
    } THRILL_ATTRIBUTE_PACKED;

    struct IndexChar {
        Index index;
        Char  ch;
    } THRILL_ATTRIBUTE_PACKED;

    uint64_t input_size = input.Size();

    DIA<Index> indices = Generate(ctx,
                                  [](size_t index) { return Index(index); },
                                  input_size);

    auto bwt =
        suffix_array
        .Map([input_size](const Index& i) {
                 if (i == Index(0))
                     return Index(input_size - 1);
                 return Index(i - 1);
             })
        .Zip(indices,
             [](const Index& text_pos, const Index& idx) {
                 return IndexRank { text_pos, idx };
             })
        .Sort([](const IndexRank& a, const IndexRank& b) {
                  return a.index < b.index;
              })
        .Zip(input,
             [](const IndexRank& text_order, const Char& ch) {
                 return IndexChar { text_order.rank, ch };
             })
        .Sort([](const IndexChar& a, const IndexChar& b) {
                  return a.index < b.index;
              })
        .Map([](const IndexChar& ic) {
                 return ic.ch;
             });

    return bwt.Collapse();
}

} // namespace suffix_sorting
} // namespace examples

#endif // !THRILL_EXAMPLES_SUFFIX_SORTING_BWT_GENERATOR_HEADER

/******************************************************************************/
