#!/usr/bin/env bash

set -euo pipefail

qml_root=${1:?usage: check-qml-import-policy.sh <qml-root>}
failed=0
style_types='ApplicationWindow|Label|ToolButton|ToolBar|ToolSeparator|MenuSeparator|Popup|MenuBar|MenuBarItem|Button|CheckBox|ComboBox|ItemDelegate|Menu|MenuItem|ProgressBar|RadioButton|ScrollBar|ScrollView|Slider|SpinBox|Switch|TabBar|TabButton|TextArea|TextField|ToolTip'
style_use="(^|[^.[:alnum:]_])(${style_types})[[:space:]]*\\{"
core_types='HoloniightPalette|HolonightTheme|HnAppearance|HnShapeProfile|HnSurfaceRole|HnCornerStyle|HnShapeKind|HnCornerMask|HnIconProvider|HnIcon|HnControlSize|HnMetrics|HnTypographyRole|HnLabel'

while IFS= read -r qml_file; do
  # This slice migrates composites and isolates Core. Standard style templates,
  # demo/gallery defaults and historical compatibility fixtures retain their policy.
  case "${qml_file}" in
    */controls/*|*/HnApplicationWindow.qml|*/HnSurfaceFrame.qml)
      if rg -q '^import (Holonight([[:space:]]|$)|QtQuick\.Controls\.)' "${qml_file}"; then
        echo "${qml_file}: composites must use runtime QtQuick.Controls as C" >&2
        failed=1
      fi
      if rg -q '\bC\.' "${qml_file}" && ! rg -q '^import QtQuick\.Controls as C$' "${qml_file}"; then
        echo "${qml_file}: C control use requires a file-local runtime import" >&2
        failed=1
      fi
      if rg -q '^import QtQuick\.Controls([[:space:]]|$)' "${qml_file}" \
          && ! rg -q '^import QtQuick\.Controls as C$' "${qml_file}"; then
        echo "${qml_file}: runtime Controls must use the C namespace" >&2
        failed=1
      fi
      if rg -q "(^|[^.[:alnum:]_])(${style_types}|Control|ButtonGroup|Overlay)([[:space:]]*\\{|\\.)" "${qml_file}"; then
        echo "${qml_file}: qualify runtime control instances, enums and attached properties" >&2
        failed=1
      fi
      ;;
    */core/*|*/HnIcon.qml|*/ComboBoxPopupGeometry.qml)
      if rg -q '^import (QtQuick\.Controls|Holonight([[:space:]]|$)|Holonight\.Controls)' "${qml_file}"; then
        echo "${qml_file}: Core and popup geometry must not import runtime Controls or a style" >&2
        failed=1
      fi
      ;;
  esac

  has_style_use=0
  rg -q "${style_use}" "${qml_file}" && has_style_use=1

  if ((has_style_use)) \
      && ! rg -q '^import (Holonight|QtQuick\.Controls(\.Basic)?)([[:space:]]|$)' "${qml_file}"; then
    echo "${qml_file}: style control used without a file-local Holonight or QtQuick.Controls import" >&2
    failed=1
  fi

  if rg -q '^import Holonight$' "${qml_file}" && (( ! has_style_use )); then
    echo "${qml_file}: unqualified Holonight import has no style-owned control consumer" >&2
    failed=1
  fi

  if [[ "${qml_file}" != */core/* ]] \
      && rg -q "\\b(${core_types})\\b" "${qml_file}" \
      && ! rg -q '^import Holonight\.Core([[:space:]]|$)' "${qml_file}"; then
    echo "${qml_file}: Core type used without a direct Holonight.Core import" >&2
    failed=1
  fi
done < <(find "${qml_root}" -type f -name '*.qml' -print | sort)

if find "${qml_root}" -type d -name holonight -print -quit | rg -q .; then
  echo "${qml_root}: lowercase holonight module directory found" >&2
  failed=1
fi

exit "${failed}"
