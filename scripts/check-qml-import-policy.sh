#!/usr/bin/env bash

set -euo pipefail

qml_root=${1:?usage: check-qml-import-policy.sh <qml-root>}
qml_root=$(cd -- "${qml_root}" && pwd)
failed=0
style_types='ApplicationWindow|Label|ToolButton|ToolBar|ToolSeparator|MenuSeparator|Popup|MenuBar|MenuBarItem|Button|CheckBox|ComboBox|ItemDelegate|Menu|MenuItem|ProgressBar|RadioButton|ScrollBar|ScrollView|Slider|SpinBox|Switch|TabBar|TabButton|TextArea|TextField|ToolTip|Control|ButtonGroup|Overlay|RangeSlider|Frame|Pane|Page|Dialog|DialogButtonBox|BusyIndicator|SwipeView|StackView|Action|ActionGroup|RoundButton|DelayButton|Tumbler|SplitView|HorizontalHeaderView|VerticalHeaderView'
style_use="(^|[^.[:alnum:]_])(${style_types})[[:space:]]*\\{"
core_types='HoloniightPalette|HolonightTheme|HnAppearance|HnShapeProfile|HnSurfaceRole|HnCornerStyle|HnShapeKind|HnCornerMask|HnIconProvider|HnIcon|HnControlSize|HnMetrics|HnTypographyRole|HnLabel'

while IFS= read -r qml_file; do
  # Applications and composites use runtime Controls. Standard Templates and
  # explicit compatibility fixtures retain their separate import boundary.
  namespace=C
  case "${qml_file}" in
    */demo/*|*/examples/controls-gallery/*) namespace=Controls ;;
  esac
  case "${qml_file}" in
    */controls/*|*/HnApplicationWindow.qml|*/HnSurfaceFrame.qml|*/demo/*|*/examples/controls-gallery/*)
      if rg -q '^import (Holonight([[:space:]]|$)|QtQuick\.Controls\.)' "${qml_file}"; then
        echo "${qml_file}: applications/composites must use runtime QtQuick.Controls as ${namespace}" >&2
        failed=1
      fi
      if rg -q "\b${namespace}\." "${qml_file}" && ! rg -q "^import QtQuick\.Controls as ${namespace}$" "${qml_file}"; then
        echo "${qml_file}: ${namespace} control use requires a file-local runtime import" >&2
        failed=1
      fi
      if rg -q '^import QtQuick\.Controls([[:space:]]|$)' "${qml_file}" \
          && ! rg -q "^import QtQuick\.Controls as ${namespace}$" "${qml_file}"; then
        echo "${qml_file}: runtime Controls must use the ${namespace} namespace" >&2
        failed=1
      fi
      if rg -q "(^|[^.[:alnum:]_])(${style_types})([[:space:]]*\\{|\\.)" "${qml_file}"; then
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
