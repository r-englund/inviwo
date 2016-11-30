/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2016 Inviwo Foundation
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

#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/filesystem.h>
#include <modules/qtwidgets/properties/filepropertywidgetqt.h>
#include <modules/qtwidgets/inviwofiledialog.h>
#include <inviwo/core/properties/propertyowner.h>

#include <warn/push>
#include <warn/ignore/all>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QList>
#include <QSettings>
#include <QUrl>
#include <QDropEvent>
#include <QMimeData>
#include <warn/pop>

namespace inviwo {

FilePropertyWidgetQt::FilePropertyWidgetQt(FileProperty* property)
    : PropertyWidgetQt(property), property_(property) {
    generateWidget();
    updateFromProperty();
}

void FilePropertyWidgetQt::generateWidget() {
    QHBoxLayout* hLayout = new QHBoxLayout();
    setSpacingAndMargins(hLayout);
    setLayout(hLayout);
    setAcceptDrops(true);

    label_ = new EditableLabelQt(this, property_);
    hLayout->addWidget(label_);

    QHBoxLayout* hWidgetLayout = new QHBoxLayout();
    hWidgetLayout->setContentsMargins(0, 0, 0, 0);
    QWidget* widget = new QWidget();
    widget->setLayout(hWidgetLayout);

    lineEdit_ = new FilePathLineEditQt(this);

    connect(lineEdit_, &QLineEdit::editingFinished, [&]() {
        // editing is done, sync property with contents
        property_->set(lineEdit_->getPath());
    });
#if defined(IVW_DEBUG)
    QObject::connect(lineEdit_, &LineEditQt::editingCanceled, [this]() {
        // undo textual changes by resetting the contents of the line edit
        ivwAssert(lineEdit_->getPath() == property_->get(), "FilePropertyWidgetQt: paths not equal after canceling edit");
    });
#endif // IVW_DEBUG

    QSizePolicy sp = lineEdit_->sizePolicy();
    sp.setHorizontalStretch(3);
    lineEdit_->setSizePolicy(sp);
    hWidgetLayout->addWidget(lineEdit_);

    auto revealButton = new QToolButton(this);
    revealButton->setIcon(QIcon(":/icons/reveal.png"));
    hWidgetLayout->addWidget(revealButton);
    connect(revealButton, &QToolButton::pressed, [&]() {
        auto dir = filesystem::directoryExists(property_->get())
                       ? property_->get()
                       : filesystem::getFileDirectory(property_->get());

        QDesktopServices::openUrl(
            QUrl(QString::fromStdString("file:///" + dir), QUrl::TolerantMode));
    });

    openButton_ = new QToolButton(this);
    openButton_->setIcon(QIcon(":/icons/open.png"));
    hWidgetLayout->addWidget(openButton_);
    connect(openButton_, SIGNAL(pressed()), this, SLOT(setPropertyValue()));

    sp = widget->sizePolicy();
    sp.setHorizontalStretch(3);
    widget->setSizePolicy(sp);
    hLayout->addWidget(widget);
}

void FilePropertyWidgetQt::setPropertyValue() {
    std::string filename{ property_->get() };

    // Setup Extensions
    std::vector<FileExtension> filters = property_->getNameFilters();
    InviwoFileDialog importFileDialog(this, property_->getDisplayName(),
                                      property_->getContentType(), filename);

    for (const auto& filter : filters) importFileDialog.addExtension(filter);

    importFileDialog.setAcceptMode(property_->getAcceptMode());
    importFileDialog.setFileMode(property_->getFileMode());

    auto ext = property_->getSelectedExtension();
    if (!ext.empty()) importFileDialog.setSelectedExtenstion(ext);

    if (importFileDialog.exec()) {
        property_->set(importFileDialog.getSelectedFile());
        property_->setSelectedExtension(importFileDialog.getSelectedFileExtension());
    }

    updateFromProperty();
}

void FilePropertyWidgetQt::dropEvent(QDropEvent* drop) {
    auto data = drop->mimeData();
    if (data->hasUrls()) {
        if(data->urls().size()>0) {
            auto url = data->urls().first();
            property_->set(url.toLocalFile().toStdString());

            drop->accept();
        }
    }
}

void FilePropertyWidgetQt::dragEnterEvent(QDragEnterEvent* event) {
    switch (property_->getAcceptMode()) {
        case AcceptMode::Save: {
            event->ignore();
            return;
        }
        case AcceptMode::Open: {
            if (event->mimeData()->hasUrls()) {
                auto data = event->mimeData();
                if (data->hasUrls()) {
                    if (data->urls().size() > 0) {
                        auto url = data->urls().first();
                        auto file = url.toLocalFile().toStdString();
                        
                        switch (property_->getFileMode()) {
                            case FileMode::AnyFile:
                            case FileMode::ExistingFile:
                            case FileMode::ExistingFiles: {
                                auto ext = toLower(filesystem::getFileExtension(file));
                                for (const auto& filter : property_->getNameFilters()) {
                                    if (filter.extension_ == ext) {
                                        event->accept();
                                        return;
                                    }
                                }
                                break;
                            }
                        
                            case FileMode::Directory:
                            case FileMode::DirectoryOnly: {
                                if(filesystem::directoryExists(file)) {
                                    event->accept();
                                    return;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            event->ignore();
            return;
        }      
    }
}


void FilePropertyWidgetQt::dragMoveEvent(QDragMoveEvent *event) {
    if(event->mimeData()->hasUrls()) event->accept();
    else event->ignore();
}

bool FilePropertyWidgetQt::requestFile() {
   setPropertyValue();
   return !property_->get().empty();
}

void FilePropertyWidgetQt::updateFromProperty() {
    lineEdit_->setPath(property_->get());
}

}  // namespace inviwo