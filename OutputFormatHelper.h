/******************************************************************************
 *
 * C++ Insights, copyright (C) by Andreas Fertig
 * Distributed under an MIT license. See LICENSE for details
 *
 ****************************************************************************/

#ifndef OUTPUT_FORMAT_HELPER_H
#define OUTPUT_FORMAT_HELPER_H
//-----------------------------------------------------------------------------

#include <utility>

#include "InsightsHelpers.h"
#include "InsightsStrCat.h"
#include "InsightsStrongTypes.h"
//-----------------------------------------------------------------------------

namespace clang::insights {
//-----------------------------------------------------------------------------

/// \brief The C++ Insights formatter.
///
/// Most of the code is handed to \ref OutputFormatHelper for easy code formatting.
class OutputFormatHelper
{
public:
    explicit OutputFormatHelper()
    : OutputFormatHelper{0}
    {
    }

    explicit OutputFormatHelper(const unsigned indent)
    : mDefaultIndent{indent}
    , mOutput{}
    {
    }

    /// \brief Returns the current position in the output buffer.
    size_t CurrentPos() const { return mOutput.length(); }

    /// \brief Insert a string before the position \c atPos
    void InsertAt(const size_t atPos, const std::string& data) { mOutput.insert(atPos, data); }

    /// \brief Set the indent level of this class to that of \c rhs.
    void SetIndent(const OutputFormatHelper& rhs)
    {
        if(&rhs != this) {
            mDefaultIndent = rhs.mDefaultIndent;
            Indent(mDefaultIndent);
        }
    }

    void RemoveIndentIncludingLastNewLine();

    /// \brief Check whether the buffer is empty.
    ///
    /// This also treats a string of just whitespaces as empty.
    bool empty() const { return mOutput.empty() || (std::string::npos == mOutput.find_first_not_of(' ', 0)); }

    /// \brief Returns a reference to the underlying string buffer.
    std::string& GetString() { return mOutput; }

    /// \brief Append a single character
    ///
    /// Append a single character to the buffer
    void Append(const char c) { mOutput += c; }

    /// \brief Append a variable number of data
    ///
    /// The \c StrCat function which is used ensures, that a \c StringRef or a char are converted appropriately.
    template<typename T, typename... Args>
    void Append(const T& first, Args&&... args)
    {
        details::StrCat(mOutput, first, std::forward<Args>(args)...);
    }

    /// \brief Same as \ref Append but adds a newline after the last argument.
    ///
    /// Append a single character to the buffer
    void AppendNewLine(const char c)
    {
        mOutput += c;
        NewLine();
    }

    /// \brief Same as \ref Append but adds a newline after the last argument.
    template<typename... Args>
    void AppendNewLine(Args&&... args)
    {
        if constexpr(0 < sizeof...(args)) {
            details::StrCat(mOutput, std::forward<Args>(args)...);
        }

        NewLine();
    }

    STRONG_BOOL(WithParameterName);

    /// \brief Append a \c ParamVarDecl array.
    ///
    /// With the parameter \c withParameterName the name will be inserted or not.
    void AppendParameterList(const ArrayRef<ParmVarDecl*> parameters,
                             const WithParameterName      withParameterName = WithParameterName::Yes);

    /// \brief Increase the current indention by \c SCOPE_INDENT
    void IncreaseIndent() { mDefaultIndent += SCOPE_INDENT; }
    /// \brief Decrease the current indention by \c SCOPE_INDENT
    void DecreaseIndent() { mDefaultIndent -= SCOPE_INDENT; }

    /// \brief Open a scope by inserting a '{' followed by an indented newline.
    void OpenScope()
    {
        Append("{");
        IncreaseIndent();
        NewLine();
    }

    STRONG_BOOL(NoNewLineBefore);
    /// \brief Close a scope by inserting a '}'
    ///
    /// With the parameter \c newLineBefore a newline after the brace can be inserted.
    void CloseScope(const NoNewLineBefore newLineBefore = NoNewLineBefore::No);

    /// \brief Similiar to \ref CloseScope only this time a ';' is inserted after the brace.
    void CloseScopeWithSemi()
    {
        CloseScope();
        Append(";");
    }

    /// \brief Append a argument list to the buffer.
    ///
    /// This function takes care of the delimiting ',' between the parameters. The lambda \c lambda is called to each
    /// argument after the comma was inserted.
    /// Usage:
    /// \code
    /// ForEachArg(parameters, [&](const auto& p) {
    /// 		// do something with p
    /// });
    /// \endcode
    template<typename T, typename Lambda>
    static inline void ForEachArg(const T& arguments, OutputFormatHelper& outputFormatHelper, Lambda&& lambda)
    {
        bool first{true};
        for(const auto& arg : arguments) {
            if(first) {
                first = false;
            } else {
                outputFormatHelper.Append(", ");
            }

            lambda(arg);
        }
    }

private:
    static constexpr unsigned SCOPE_INDENT{2};
    unsigned                  mDefaultIndent;
    std::string               mOutput;

    void Indent(unsigned count);
    void NewLine()
    {
        mOutput += '\n';
        Indent(mDefaultIndent);
    }

    void RemoveIndent();

    template<typename T, typename Lambda>
    inline void ForEachArg(const T& arguments, Lambda&& lambda)
    {
        ForEachArg(arguments, *this, lambda);
    }
};
//-----------------------------------------------------------------------------

}  // namespace clang::insights
#endif /* OUTPUT_FORMAT_HELPER_H */
