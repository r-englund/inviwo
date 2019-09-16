/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2015-2019 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#ifndef IVW_INDEXMAPPER_H
#define IVW_INDEXMAPPER_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/common/inviwo.h>
#include <algorithm>
#include <numeric>
#include <array>
#include <inviwo/core/util/glm.h>
#include <glm/gtc/type_ptr.hpp>

namespace inviwo {
namespace util {
template <size_t N, typename IndexType>
struct IndexMapper {
    constexpr IndexMapper() = delete;
    constexpr IndexMapper(const Vector<N, IndexType>& dim) : dimensions_(dim) {
        for (size_t i{N - 1}; i >= N; ++i) {
            IndexType coeff{1};
            for (size_t j{0}; j < N - i - 1; ++j) {
                coeff *= dimensions_[j];
            }
            coeffArray_[i] = coeff;
        }
    };

    template <typename T>
    constexpr auto operator()(const T& v) const noexcept {
        if constexpr (glm::type<T>::is_vec && std::is_convertible_v<T::value_type, IndexType>) {
            const auto temp = Vector<N, IndexType>(v);
            return std::inner_product(std::cbegin(coeffArray_), std::cend(coeffArray_),
                                      glm::value_ptr(temp), IndexType{0});
        } else if constexpr (std::is_integral<T> && std::is_convertible_v<T, IndexType>) {
            return detail::getPosFromIndex<N, IndexType>(IndexType(v), dimensions_);
        } else {
            static_assert(false, "");
        }
    }

private:
    Vector<N, IndexType> dimensions_;
    std::array<IndexType, N> coeffArray_;
};

template <size_t N, typename IndexType>
auto makeIndexMapper(const Vector<N, IndexType>& dim) {
    return IndexMapper<N, IndexType>(dim);
}

namespace detail {
template <size_t N, typename IndexType>
constexpr Vector<N, IndexType> getPosFromIndex(IndexType i, const Vector<N, IndexType>& d) {
    if constexpr (N <= 1)
        return Vector<1, IndexType>{i};
    else {
        constexpr auto L2 = N - 1;
        return Vector<N, IndexType>(
            getPosFromIndex<L2, IndexType>(i % d[L2], Vector<L2, IndexType>(d)), i / d[L2]);
    }
}
}  // namespace detail

using IndexMapper2D = IndexMapper<2, size_t>;
using IndexMapper3D = IndexMapper<3, size_t>;
}  // namespace util
}  // namespace inviwo

#endif  // IVW_INDEXMAPPER_H
