/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2015-2020 Inviwo Foundation
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

#ifndef IVW_NIFTIREADER_H
#define IVW_NIFTIREADER_H

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/datastructures/diskrepresentation.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>
#include <inviwo/core/io/datareader.h>
#include <inviwo/core/io/datareaderexception.h>
#include <inviwo/core/util/indexmapper.h>
#include <modules/nifti/niftimoduledefine.h>

#include <array>

namespace inviwo {

/**
 * \class NiftiReader
 * \brief Volume data reader for Nifti-1 files.
 *
 */
class IVW_MODULE_NIFTI_API NiftiReader
    : public DataReaderType<std::vector<std::shared_ptr<Volume>>> {
public:
    using VolumeSequence = std::vector<std::shared_ptr<Volume>>;
    NiftiReader();
    NiftiReader(const NiftiReader& rhs) = default;
    NiftiReader& operator=(const NiftiReader& that) = default;
    virtual NiftiReader* clone() const override;

    virtual ~NiftiReader() = default;

    virtual std::shared_ptr<VolumeSequence> readData(const std::string& filePath) override;

    /**
     * \brief Convert from Nifti defined data types to inviwo DataFormat.
     *
     * @param niftiDataType nifti_image::datatype.
     * @return Equivalent data type, null if not found.
     */
    static const DataFormatBase* niftiDataTypeToInviwoDataFormat(int niftiDataType);
};


}  // namespace inviwo

#endif  // IVW_NIFTIREADER_H
