#!/usr/bin/env bash

set -euo pipefail

qml_root=${1:?usage: check-qml-import-policy.sh <qml-root>}
failed=0
style_types='Button|CheckBox|ComboBox|ItemDelegate|Menu|MenuItem|ProgressBar|RadioButton|ScrollBar|ScrollView|Slider|SpinBox|Switch|TabBar|TabButton|TextArea|TextField|ToolTip'
style_use="(^|[^.[:alnum:]_])(${style_types})[[:space:]]*\\{"
core_types='HoloniightPalette|HolonightTheme|HnAppearance|HnShapeProfile|HnSurfaceRole|HnCornerStyle|HnShapeKind|HnCornerMask|HnIconProvider|HnIcon|HnControlSize|HnMetrics|HnTypographyRole|HnLabel'

while IFS= read -r qml_file; do
  has_style_use=0
  rg -q "${style_use}" "${qml_file}" && has_style_use=1

  if ((has_style_use)) \
      && ! rg -q '^import (Holonight|QtQuick\.Controls)([[:space:]]|$)' "${qml_file}"; then
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
