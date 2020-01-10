//===-- lib/semantics/check-deallocate.cc ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "check-deallocate.h"
#include "expression.h"
#include "tools.h"
#include "../parser/message.h"
#include "../parser/parse-tree.h"

namespace Fortran::semantics {

void DeallocateChecker::Leave(const parser::DeallocateStmt &deallocateStmt) {
  for (const parser::AllocateObject &allocateObject :
      std::get<std::list<parser::AllocateObject>>(deallocateStmt.t)) {
    std::visit(
        common::visitors{
            [&](const parser::Name &name) {
              auto const *symbol{name.symbol};
              if (context_.HasError(symbol)) {
                // already reported an error
              } else if (!IsVariableName(*symbol)) {
                context_.Say(name.source,
                    "name in DEALLOCATE statement must be a variable name"_err_en_US);
              } else if (!IsAllocatableOrPointer(*symbol)) {  // C932
                context_.Say(name.source,
                    "name in DEALLOCATE statement must have the ALLOCATABLE or POINTER attribute"_err_en_US);
              } else {
                context_.CheckDoVarRedefine(name);
              }
            },
            [&](const parser::StructureComponent &structureComponent) {
              evaluate::ExpressionAnalyzer analyzer{context_};
              if (MaybeExpr checked{analyzer.Analyze(structureComponent)}) {
                if (!IsAllocatableOrPointer(
                        *structureComponent.component.symbol)) {  // C932
                  context_.Say(structureComponent.component.source,
                      "component in DEALLOCATE statement must have the ALLOCATABLE or POINTER attribute"_err_en_US);
                }
              }
            },
        },
        allocateObject.u);
  }
  bool gotStat{false}, gotMsg{false};
  for (const parser::StatOrErrmsg &deallocOpt :
      std::get<std::list<parser::StatOrErrmsg>>(deallocateStmt.t)) {
    std::visit(
        common::visitors{
            [&](const parser::StatVariable &) {
              if (gotStat) {
                context_.Say(
                    "STAT may not be duplicated in a DEALLOCATE statement"_err_en_US);
              }
              gotStat = true;
            },
            [&](const parser::MsgVariable &) {
              if (gotMsg) {
                context_.Say(
                    "ERRMSG may not be duplicated in a DEALLOCATE statement"_err_en_US);
              }
              gotMsg = true;
            },
        },
        deallocOpt.u);
  }
}
}  // namespace Fortran::semantics
