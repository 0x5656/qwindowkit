#include "windowagentbase.h"
#include "windowagentbase_p.h"

#include "qwkglobal_p.h"

#if defined(Q_OS_WINDOWS) && !defined(QWINDOWKIT_FORCE_QT_WINDOW_CONTEXT)
#  include "win32windowcontext_p.h"
#elif defined(Q_OS_MAC) && !defined(QWINDOWKIT_FORCE_QT_WINDOW_CONTEXT)
#  include "cocoawindowcontext_p.h"
#else
#  include "qtwindowcontext_p.h"
#endif

Q_LOGGING_CATEGORY(qWindowKitLog, "qwindowkit")

namespace QWK {

    WindowAgentBasePrivate::WindowContextFactoryMethod
        WindowAgentBasePrivate::windowContextFactoryMethod = nullptr;

    WindowAgentBasePrivate::WindowAgentBasePrivate() : q_ptr(nullptr), context(nullptr) {
    }

    WindowAgentBasePrivate::~WindowAgentBasePrivate() = default;

    void WindowAgentBasePrivate::init() {
    }

    AbstractWindowContext *WindowAgentBasePrivate::createContext() const {
        if (windowContextFactoryMethod) {
            return windowContextFactoryMethod();
        }

#if defined(Q_OS_WINDOWS) && !defined(QWINDOWKIT_FORCE_QT_WINDOW_CONTEXT)
        return new Win32WindowContext();
#elif defined(Q_OS_MAC) && !defined(QWINDOWKIT_FORCE_QT_WINDOW_CONTEXT)
        return new CocoaWindowContext();
#else
        return new QtWindowContext();
#endif
    }

    bool WindowAgentBasePrivate::setup(QObject *host, WindowItemDelegate *delegate) {
        std::unique_ptr<AbstractWindowContext> ctx(createContext());
        if (!ctx->setup(host, delegate)) {
            return false;
        }
        context = std::move(ctx);
        return true;
    }

    WindowAgentBase::~WindowAgentBase() = default;

    void WindowAgentBase::showSystemMenu(const QPoint &pos) {
        Q_D(WindowAgentBase);
        d->context->showSystemMenu(pos);
    }

    void WindowAgentBase::startSystemMove(const QPoint &pos) {
        Q_D(WindowAgentBase);
        auto win = d->context->window();
        if (!win) {
            return;
        }

        Q_UNUSED(pos)
        win->startSystemMove();
    }

    void WindowAgentBase::startSystemResize(Qt::Edges edges, const QPoint &pos) {
        Q_D(WindowAgentBase);
        auto win = d->context->window();
        if (!win) {
            return;
        }

        Q_UNUSED(pos)
        win->startSystemResize(edges);
    }

    void WindowAgentBase::centralize() {
    }

    void WindowAgentBase::raise() {
    }

    WindowAgentBase::WindowAgentBase(WindowAgentBasePrivate &d, QObject *parent)
        : QObject(parent), d_ptr(&d) {
        d.q_ptr = this;

        d.init();
    }

}