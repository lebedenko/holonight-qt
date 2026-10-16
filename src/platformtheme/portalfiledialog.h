// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QPointer>
#include <qpa/qplatformdialoghelper.h>

class PortalFileRequest;
struct PortalFileSelection;

// Private to the platform plugin. Constructing a helper never contacts D-Bus.
class PortalFileDialog final : public QPlatformFileDialogHelper {
  Q_OBJECT
 public:
  PortalFileDialog() = default;
  Q_DISABLE_COPY_MOVE(PortalFileDialog)
  ~PortalFileDialog() override;
  [[nodiscard]] bool defaultNameFilterDisables() const override { return false; }
  bool show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow* parent) override;
  void exec() override;
  void hide() override;
  void setDirectory(const QUrl& directory) override { directory_ = directory; }
  [[nodiscard]] QUrl directory() const override { return directory_; }
  void selectFile(const QUrl& file) override { files_ = {file}; }
  [[nodiscard]] QList<QUrl> selectedFiles() const override { return files_; }
  void setFilter() override {}
  void selectNameFilter(const QString& filter) override { name_filter_ = filter; }
  [[nodiscard]] QString selectedNameFilter() const override { return name_filter_; }
  void selectMimeTypeFilter(const QString& filter) override { mime_filter_ = filter; }
  [[nodiscard]] QString selectedMimeTypeFilter() const override { return mime_filter_; }

 private:
  PortalFileSelection filterSelection();
  QVariantMap portalOptions(const QString& token, Qt::WindowModality modality, const PortalFileSelection& selection);
  void complete(bool accepted, const QVariantMap& results, const PortalFileSelection& selection);

  QPointer<PortalFileRequest> request_;
  QUrl directory_;
  QList<QUrl> files_;
  QString name_filter_;
  QString mime_filter_;
};
