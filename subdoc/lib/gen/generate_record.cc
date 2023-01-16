// Copyright 2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "subdoc/lib/gen/generate_record.h"

#include "subdoc/lib/database.h"
#include "subdoc/lib/gen/files.h"
#include "subdoc/lib/gen/html_writer.h"
#include "subdoc/lib/gen/options.h"

namespace subdoc::gen {

void generate_record(const RecordElement& element,
                     const Options& options) noexcept {
  const std::filesystem::path path = construct_html_file_path(
      options.output_root, element.namespace_path.as_ref(),
      element.class_path.as_ref(), element.name);
  std::filesystem::create_directories(path.parent_path());
  auto html = HtmlWriter(open_file_for_writing(path).unwrap());
  {
    auto head = html.open_head();
    {
      auto title = head.open_title();
      title.write_text(element.name);
    }
    {
      auto default_stylesheet_link = head.open_link();
      default_stylesheet_link.add_rel("stylesheet");
      default_stylesheet_link.add_href(options.default_stylesheet_path);
    }
  }
  auto body = html.open_body();
  {
    auto type_div = body.open_div();
    type_div.add_class("type");
    {
      auto record_header_div = type_div.open_div();
      record_header_div.add_class("section-header");
      record_header_div.add_class("type");
      {
        auto record_type_span = record_header_div.open_span();
        record_type_span.write_text(
            friendly_record_type_name(element.record_type, true));
      }
      {
        auto name_anchor = record_header_div.open_a();
        name_anchor.add_href("#");
        name_anchor.add_class("type-name");
        name_anchor.write_text(element.name);
      }
    }
    {
      auto type_sig_div = type_div.open_div();
      type_sig_div.add_class("type-signature");
      {
        auto record_type_span = type_sig_div.open_span();
        std::string record_type_name =
            friendly_record_type_name(element.record_type, false);
        record_type_span.add_class(record_type_name);
        record_type_span.write_text(record_type_name);
      }
      {
        auto name_span = type_sig_div.open_span();
        name_span.add_class("type-name");
        name_span.write_text(element.name);
      }
      {
        auto record_body_div = type_sig_div.open_div();
        record_body_div.add_class("record-body");
        record_body_div.write_text("{ /* body omitted */ }");
      }
    }
    {
      auto desc_div = type_div.open_div();
      desc_div.add_class("description");
      desc_div.write_text(element.comment.raw_text);
    }

    // TODO: List fields, methods, concepts, etc.

    // TODO: Add sorting to Vec.
    sus::Vec<sus::Tuple<std::string_view, UniqueSymbol>> sorted_static_fields;
    sus::Vec<sus::Tuple<std::string_view, UniqueSymbol>> sorted_fields;
    for (const auto& [field_symbol, field_element] : element.fields) {
      switch (field_element.is_static) {
        case FieldElement::Static:
          sorted_static_fields.push(
              sus::tuple(field_element.name, field_symbol));
          break;
        case FieldElement::NonStatic:
          sorted_fields.push(sus::tuple(field_element.name, field_symbol));
          break;
      }
    }
    if (sorted_static_fields.len() > 0u) {
      std::sort(sorted_static_fields.as_mut_ptr(),
                sorted_static_fields.as_mut_ptr() +
                    sorted_static_fields.len().primitive_value,
                [](const sus::Tuple<std::string_view, UniqueSymbol>& a,
                   const sus::Tuple<std::string_view, UniqueSymbol>& b) {
                  return a.get_ref<0>() < b.get_ref<0>();
                });
    }
    if (sorted_fields.len() > 0u) {
      std::sort(
          sorted_fields.as_mut_ptr(),
          sorted_fields.as_mut_ptr() + sorted_fields.len().primitive_value,
          [](const sus::Tuple<std::string_view, UniqueSymbol>& a,
             const sus::Tuple<std::string_view, UniqueSymbol>& b) {
            return a.get_ref<0>() < b.get_ref<0>();
          });
    }

    if (sorted_static_fields.len() > 0u) {
      auto section_div = type_div.open_div();
      section_div.add_class("section");
      {
        auto fields_header_div = section_div.open_div();
        fields_header_div.add_class("section-header");
        fields_header_div.add_class("fields");
        fields_header_div.add_class("static");
        fields_header_div.write_text("Static Data Members");
      }
      {
        for (auto&& [name, field_symbol] : sorted_static_fields) {
          auto field_div = section_div.open_div();
          field_div.add_class("section-item");
          field_div.add_class("fields");
          field_div.add_class("static");

          const FieldElement& fe = element.fields.at(field_symbol);
          {
            auto field_type_span = field_div.open_span();
            field_type_span.add_class("static");
            field_type_span.write_text("static");
          }
          if (fe.is_const) {
            auto field_type_span = field_div.open_span();
            field_type_span.add_class("const");
            field_type_span.write_text("const");
          }
          if (fe.is_volatile) {
            auto field_type_span = field_div.open_span();
            field_type_span.add_class("volatile");
            field_type_span.write_text("volatile");
          }
          {
            auto field_type_span = field_div.open_span();
            field_type_span.add_class("type-name");
            field_type_span.write_text(fe.type_name);
          }
          {
            auto field_name_anchor = field_div.open_a();
            std::string anchor = std::string("field.static.") + fe.name;
            field_name_anchor.add_name(anchor);
            field_name_anchor.add_href(std::string("#") + anchor);
            field_name_anchor.add_class("field-name");
            field_name_anchor.write_text(fe.name);
          }
          {
            auto desc_div = field_div.open_div();
            desc_div.add_class("description");
            if (fe.has_comment()) {
              desc_div.write_text(fe.comment.raw_text);
            }
          }
        }
      }
    }

    if (sorted_fields.len() > 0u) {
      auto section_div = type_div.open_div();
      section_div.add_class("section");
      {
        auto fields_header_div = section_div.open_div();
        fields_header_div.add_class("section-header");
        fields_header_div.add_class("fields");
        fields_header_div.add_class("nonstatic");
        fields_header_div.write_text("Data Members");
      }
      {
        for (auto&& [name, field_symbol] : sorted_fields) {
          auto field_div = section_div.open_div();
          field_div.add_class("section-item");
          field_div.add_class("fields");
          field_div.add_class("nonstatic");

          const FieldElement& fe = element.fields.at(field_symbol);
          if (fe.is_const) {
            auto field_type_span = field_div.open_span();
            field_type_span.add_class("const");
            field_type_span.write_text("const");
          }
          if (fe.is_volatile) {
            auto field_type_span = field_div.open_span();
            field_type_span.add_class("volatile");
            field_type_span.write_text("volatile");
          }
          {
            auto field_type_span = field_div.open_span();
            field_type_span.add_class("type-name");
            field_type_span.write_text(fe.type_name);
          }
          {
            auto field_name_anchor = field_div.open_a();
            std::string anchor = std::string("field.") + fe.name;
            field_name_anchor.add_name(anchor);
            field_name_anchor.add_href(std::string("#") + anchor);
            field_name_anchor.add_class("field-name");
            field_name_anchor.write_text(fe.name);
          }
          {
            auto desc_div = field_div.open_div();
            desc_div.add_class("description");
            if (fe.has_comment()) {
              desc_div.write_text(fe.comment.raw_text);
            }
          }
        }
      }
    }
  }
}

}  // namespace subdoc::gen