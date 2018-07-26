/******************************************************************************
 *
 * C++ Insights, copyright (C) by Andreas Fertig
 * Distributed under an MIT license. See LICENSE for details
 *
 ****************************************************************************/

#include "UserDefinedLiteralHandler.h"
#include "CodeGenerator.h"
#include "DPrint.h"
#include "InsightsMatchers.h"
//-----------------------------------------------------------------------------

using namespace clang;
using namespace clang::ast_matchers;
//-----------------------------------------------------------------------------

namespace clang::insights {

UserDefinedLiteralHandler::UserDefinedLiteralHandler(Rewriter& rewrite, MatchFinder& matcher)
: InsightsBase(rewrite)
{
    matcher.addMatcher(
        userDefinedLiteral(unless(anyOf(isExpansionInSystemHeader(),
                                        isMacroOrInvalidLocation(),
                                        hasAncestor(userDefinedLiteral()),
                                        isTemplate,
                                        /* if we match the top-most CXXOperatorCallExpr we will see all
                                           descendants. So filter them here to avoid getting them multiple times */
                                        hasAncestor(functionDecl()),
                                        hasAncestor(implicitCastExpr(hasMatchingCast())))))
            .bind("udl"),
        this);
}
//-----------------------------------------------------------------------------

void UserDefinedLiteralHandler::run(const MatchFinder::MatchResult& result)
{
    if(const auto* udl = result.Nodes.getNodeAs<UserDefinedLiteral>("udl")) {
        SKIP_IF_ALREADY_SEEN(udl);

        OutputFormatHelper outputFormatHelper{};
        CodeGenerator      codeGenerator{outputFormatHelper};
        codeGenerator.InsertArg(udl);

        DPrint("udl repl: %s\n", outputFormatHelper.GetString());
        mRewrite.ReplaceText(udl->getSourceRange(), outputFormatHelper.GetString());
    }
}
//-----------------------------------------------------------------------------

}  // namespace clang::insights
