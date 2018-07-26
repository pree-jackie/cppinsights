/******************************************************************************
 *
 * C++ Insights, copyright (C) by Andreas Fertig
 * Distributed under an MIT license. See LICENSE for details
 *
 ****************************************************************************/

#ifndef INSIGHTS_MATCHERS_H
#define INSIGHTS_MATCHERS_H

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

#include "InsightsHelpers.h"
//-----------------------------------------------------------------------------

namespace clang {
namespace ast_matchers {
const internal::VariadicDynCastAllOfMatcher<Type, DecltypeType>      decltypeType;
const internal::VariadicDynCastAllOfMatcher<Decl, DecompositionDecl> decompositionDecl;  // DecompositionDecl
//-----------------------------------------------------------------------------

/* don't replace stuff in template definitions */
static const auto isTemplate = anyOf(hasAncestor(classTemplateDecl()),
                                     hasAncestor(functionTemplateDecl()),
                                     hasAncestor(classTemplateSpecializationDecl()));
//-----------------------------------------------------------------------------

/// \brief Shut up a unused variable warnings
#define SILENCE                                                                                                        \
    (void)Finder;                                                                                                      \
    (void)Builder

// \brief Matches AST nodes of type \c FunctionDecl which is a template instantiation.
AST_MATCHER(FunctionDecl, isTemplateInstantiationPlain)
{
    SILENCE;
    return Node.isTemplateInstantiation();
}

static inline bool IsMatchingCast(const CastKind kind)
{
    switch(kind) {
        case CastKind::CK_Dependent: [[fallthrough]];
        case CastKind::CK_IntegralCast: [[fallthrough]];
        case CastKind::CK_IntegralToBoolean: [[fallthrough]];
        case CastKind::CK_IntegralToPointer: [[fallthrough]];
        case CastKind::CK_PointerToIntegral: [[fallthrough]];
        case CastKind::CK_BitCast: [[fallthrough]];
        case CastKind::CK_UncheckedDerivedToBase: [[fallthrough]];
        case CastKind::CK_ToUnion: [[fallthrough]];
        case CastKind::CK_UserDefinedConversion: [[fallthrough]];
        case CastKind::CK_AtomicToNonAtomic: [[fallthrough]];
        case CastKind::CK_DerivedToBase: [[fallthrough]];
        case CastKind::CK_FloatingCast: [[fallthrough]];
        case CastKind::CK_IntegralToFloating: [[fallthrough]];
        case CastKind::CK_FloatingToIntegral: [[fallthrough]];
        /* these are implicit conversions. We get them right, but they may end up in a compiler internal type,
         * which leads to compiler errors */
        // case CastKind::CK_NoOp:
        case CastKind::CK_NonAtomicToAtomic: return true;
        default: return false;
    }
}
//-----------------------------------------------------------------------------

/// \brief Matches AST nodes with a matching cast kind for our ImplicitCastExpr
AST_MATCHER(ImplicitCastExpr, hasMatchingCast)
{
    SILENCE;

    return IsMatchingCast(Node.getCastKind());
}

AST_POLYMORPHIC_MATCHER(isMacroOrInvalidLocation, AST_POLYMORPHIC_SUPPORTED_TYPES(Decl, Stmt))
{
    SILENCE;

    return (insights::IsMacroLocation(Node.getLocStart()) || insights::IsInvalidLocation(Node.getLocStart()));
}

}  // namespace ast_matchers
}  // namespace clang
//-----------------------------------------------------------------------------

#endif /* INSIGHTS_MATCHERS_H */
