/*
Copyright (c) 2017 Pavel Perina
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/// \file	fixed_aspect_ratio_qlayout.h
/// \brief	Header only implementation of AspectRatioSingleItemLayout

#ifndef ASPECTRATIO_SINGLEITEM_LAYOUT_H
#define ASPECTRATIO_SINGLEITEM_LAYOUT_H

#include <QtWidgets/QLayout>
#include <QDebug>

/// \brief Layout that keeps single item and maintains given aspect ratio
class AspectSingleLayout : public QLayout {
public:

    /// \brief Constructor
    AspectSingleLayout(QWidget *parent = nullptr, double aspectRatio = 16.0/9.0)
        : QLayout(parent)
        , m_item(nullptr)
        , m_aspectRatio(aspectRatio)
    {
    }

    /// \brief Destructor
    /// \warning Let's hope we're responsible for deleting item
    ~AspectSingleLayout()
    {
        delete m_item;
    }

    /// \brief Set aspect ratio to keep
    /// \todo Invalidate geometry
    void setAspectRatio(double aspectRatio)
    {
        m_aspectRatio = aspectRatio;
    }

    /// \brief Return number of items
    int count() const override
    {
        return m_item != nullptr ? 1 : 0;
    }

    /// \brief Return item at given index, nullptr if index is out of range
    QLayoutItem *itemAt(int i) const override
    {
        return i == 0 ? m_item : nullptr;
    }

    /// \brief Take item at index. Now caller is responsible for deletion, we no longer own it
    QLayoutItem *takeAt(int) override
    {
        QLayoutItem *retval = m_item;
        m_item = nullptr;
        return retval;
    }

    /// \brief Returns directions where we want to expand beyond sizeHint()
    Qt::Orientations expandingDirections() const override
    {
        // we'd like grow beyond sizeHint()
        return  Qt::Horizontal | Qt::Vertical;
    }

    /// \brief We want to limit height based on width
    bool hasHeightForWidth() const override
    {
        return false;
    }

    /// \brief We want that much height for given \a width
    int heightForWidth(int width) const override
    {
        int height = (width - 2 * margin()) / m_aspectRatio + 2 * margin();
        return height;
    }

    /// \brief Set geometry of our only widget
    void setGeometry(const QRect &rect) override
    {
        QLayout::setGeometry(rect);
        if (m_item) {
            QWidget *wdg = m_item->widget();
            int availW = rect.width() - 2 * margin();
            int availH = rect.height() - 2 * margin();
            int w, h;
            h = availH;
            w = h * m_aspectRatio;
            if (w > availW) {
                // fill width
                w = availW;
                h = w / m_aspectRatio;
                int y;
                if (m_item->alignment() & Qt::AlignTop)
                    y = margin();
                else if (m_item->alignment() & Qt::AlignBottom)
                    y = rect.height() - margin() - h;
                else
                    y = margin() + (availH - h) / 2;
                wdg->setGeometry(rect.x() + margin(), rect.y() + y, w, h);
            }
            else {
                int x;
                if (m_item->alignment() & Qt::AlignLeft)
                    x = margin();
                else if (m_item->alignment() & Qt::AlignRight)
                    x = rect.width() - margin() - w;
                else
                    x = margin() + (availW - w) / 2;
                wdg->setGeometry(rect.x() + x, rect.y() + margin(), w, h);
            }
        }
    }

    QSize sizeHint() const override
    {
        const int margins = 2 * margin();
        return m_item ? m_item->sizeHint() + QSize(margins, margins) : QSize(margins, margins);
    }

    QSize minimumSize() const override
    {
        const int margins = 2 * margin();
        return m_item ? m_item->minimumSize() + QSize(margins, margins) : QSize(margins,margins);
    }

    /// \brief Add item, this removes existing from container
    void addItem(QLayoutItem *item) override
    {
        delete m_item;
        m_item = item;
        item->setAlignment(0);
    }

private:

    QLayoutItem *m_item; // at most one :-)

    double m_aspectRatio;

};

#endif // ASPECTRATIO_SINGLEITEM_LAYOUT_H
