/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "view/image_viewer_proxy.h"
#include "view/widget/image_viewer.h"

#include <DLog>

ImageViewerProxy::ImageViewerProxy(ImageViewer *viewer,
                                   QObject *parent)
    : QObject(parent)
    , viewer_(viewer)
{
}

ImageViewerProxy::~ImageViewerProxy()
{
}

/**
 * @brief ImageViewerProxy::open
 * @param filepath 图片的绝对路径
 * 在窗口中显示图片
 */
void ImageViewerProxy::open(const QString &filepath)
{
    Q_ASSERT(viewer_ != nullptr);
    Q_UNUSED(filepath);
    viewer_->open(filepath);
}

/**
 * @brief ImageViewerProxy::openHttpUrl
 * @param httpUrl 链接地址
 * 打开外部链接
 */
void ImageViewerProxy::openHttpUrl(const QString &httpUrl)
{
    QDesktopServices::openUrl(QUrl(httpUrl));
}
