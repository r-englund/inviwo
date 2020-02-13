/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2019 Inviwo Foundation
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

#ifndef IVW_NETWORKEDITORVIEW_H
#define IVW_NETWORKEDITORVIEW_H

#include <inviwo/qt/editor/inviwoqteditordefine.h>
#include <inviwo/qt/editor/networkeditor.h>
#include <inviwo/core/network/workspacemanager.h>

#include <warn/push>
#include <warn/ignore/all>
#include <QGraphicsView>
#include <QImage>
#include <QPushButton>
#include <warn/pop>

#include <inviwo/qt/editor/inviwomainwindow.h>
#include <modules/qtwidgets/inviwoqtutils.h>
#include <modules/qtwidgets/qstringhelper.h>

class QDropEvent;
class QDragEnterEvent;

namespace inviwo {

class InviwoMainWindow;
class MenuItem;
class NetworkSearch;
class TextLabelOverlay;

class IVW_QTEDITOR_API NetworkProfilingInfoWidget : public QPushButton,
                                                    public ProcessorNetworkEvaluationObserver {
public:
    NetworkProfilingInfoWidget(InviwoMainWindow* parent)
        : QPushButton(QIcon(":/svgicons/timer.svg"), "0", parent) {
        parent->getInviwoApplication()->getProcessorNetworkEvaluator()->addObserver(this);

        this->setDisabled(true);

        this->setStyleSheet("border: 0px; color: #000000; ");
    }

    bool event(QEvent* event) override {
        if (event->type() == QEvent::ToolTip) {
            setToolTip(utilqt::toLocalQString(log()));
        }
        return QPushButton::event(event);
    }

    virtual void onProcessorNetworkEvaluationBegin() override {
        if (clock_.isRunning()) {
            LogWarn("Clock is already running");
            clock_.stop();
        }

        clock_.reset();
        clock_.start();
    };
    virtual void onProcessorNetworkEvaluationEnd() override {
        if (!clock_.isRunning()) {
            LogWarn("Clock is already running");
        } else {
            clock_.stop();
            evaluationTimes_.push_back(clock_.getElapsedMilliseconds());
            auto N = evaluationTimes_.size();
            auto str = QStringHelper<decltype(N)>::toLocaleString(QLocale::system(), N);
            setText(str);
        }
    };

    void clear() { evaluationTimes_.clear(); }

    Document log() const {
        using P = Document::PathComponent;
        using H = utildoc::TableBuilder::Header;

        Document doc;

        const auto N = evaluationTimes_.size();
        doc.append("style", "*{color: #9d9995;}");
        doc.append("b", "Network Evaluation Statistics");

        utildoc::TableBuilder tb(doc.handle(), P::end());

        tb("Number of evaluations", N);

        if (N > 0) {
            auto [min, max] = std::minmax_element(evaluationTimes_.begin(), evaluationTimes_.end());

            auto meanTime =
                std::accumulate(evaluationTimes_.begin(), evaluationTimes_.end(), 0.0) / N;

            auto varFunc = [mean = meanTime](auto accum, auto sample) {
                const auto temp = sample - mean;
                return accum + (temp * temp);
            };

            auto variance =
                std::accumulate(evaluationTimes_.begin(), evaluationTimes_.end(), 0.0, varFunc) /
                (N - 1);

            tb("Mean evaluation time (ms)", meanTime);
            tb("Standard deviation (ms)", std::sqrt(variance));
            tb("Min evaluatin time (ms)", *min);
            tb("Max evaluatin time (ms)", *max);

            size_t last = std::min(N, size_t(12));
            auto last12Times =
                joinString(evaluationTimes_.rbegin(), evaluationTimes_.rbegin() + last, " ms, ");
            tb(fmt::format("Last {} evaluations", last),
               last12Times + (last != N ? ("ms, ...]") : ("ms]")));
        }

        return doc;
    }

private:
    Clock clock_;
    std::vector<double> evaluationTimes_;
};

class IVW_QTEDITOR_API NetworkEditorView : public QGraphicsView, public NetworkEditorObserver {
public:
    NetworkEditorView(NetworkEditor* networkEditor, InviwoMainWindow* parent = nullptr);
    ~NetworkEditorView();

    void hideNetwork(bool);
    void fitNetwork();
    virtual void onNetworkEditorFileChanged(const std::string& newFilename) override;

    void exportViewToFile(const QString& filename, bool entireScene, bool backgroundVisible);
    QImage exportViewToImage(bool entireScene, bool backgroundVisible, QSize size = QSize());

    TextLabelOverlay& getOverlay() const;
    NetworkSearch& getNetworkSearch() const;

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent* e) override;
    virtual void resizeEvent(QResizeEvent* re) override;
    virtual void wheelEvent(QWheelEvent* e) override;

    virtual void keyPressEvent(QKeyEvent* keyEvent) override;
    virtual void keyReleaseEvent(QKeyEvent* keyEvent) override;
    virtual void focusOutEvent(QFocusEvent*) override;

private:
    void zoom(double dz);
    virtual void onSceneSizeChanged() override;

    InviwoMainWindow* mainwindow_;
    NetworkEditor* editor_;
    NetworkSearch* search_;
    TextLabelOverlay* overlay_;
#ifdef IVW_PROFILING
    NetworkProfilingInfoWidget* networkProfilingInfo_;
#endif

    ivec2 scrollPos_;
    WorkspaceManager::DeserializationHandle loadHandle_;
    WorkspaceManager::ClearHandle clearHandle_;
    std::shared_ptr<MenuItem> editActionsHandle_;
};

}  // namespace inviwo

#endif  // IVW_NETWORKEDITORVIEW_H
