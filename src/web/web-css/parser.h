#pragma once

#include "lexer.h"

namespace Web::Css {

// MARK: Sst -------------------------------------------------------------------

// The SST (Skeleton Syntax Tree) is an intermediary representation of the CSS used to build the real syntaxic tree
// We have all the block and declarations here but didn't interpreted it because we lacked context in the previous parse step
// when we have this representation we can parse it a last time and interpret the different blocks and functions to build the CSSOM
// The name come from: https://people.csail.mit.edu/jrb/Projects/dexprs.pdf chapter 3.4

struct Sst;

using Content = Vec<Sst>;

#define FOREACH_SST(SST) \
    SST(RULE)            \
    SST(FUNC)            \
    SST(DECL)            \
    SST(LIST)            \
    SST(TOKEN)           \
    SST(BLOCK)

struct Sst {
    enum struct Type {
#define ITER(NAME) NAME,
        FOREACH_SST(ITER)
#undef ITER
    };
    using enum Type;

    Type type;
    Token token = Token(Token::NIL);
    Opt<Box<Sst>> prefix{};
    Content content{};

    Sst(Type type) : type(type) {}

    Sst(Token token) : type(TOKEN), token(token) {}

    Sst(Content content) : type(LIST), content(content) {}

    void repr(Io::Emit &e) const;

    bool operator==(Type type) const {
        return this->type == type;
    }

    bool operator==(Token::Type const &type) const {
        return this->type == TOKEN and
               token.type == type;
    }
};

Str toStr(Sst::Type type);

// MARK: Parser ----------------------------------------------------------------

Content consumeRuleList(Lexer &lex, bool topLevel);

Sst consumeAtRule(Lexer &lex);

Opt<Sst> consumeRule(Lexer &lex);

Content consumeDeclarationList(Lexer &lex);

Content consumeDeclarationBlock(Lexer &lex);

bool declarationAhead(Lexer lex);

Opt<Sst> consumeDeclaration(Lexer &lex);

Sst consumeComponentValue(Lexer &lex);

Sst consumeBlock(Lexer &lex, Token::Type term);

Sst consumeFunc(Lexer &lex);

} // namespace Web::Css

Reflectable$(Web::Css::Sst, type, token, prefix, content);

template <>
struct Karm::Io::Formatter<Web::Css::Sst::Type> {
    Res<usize> format(Io::TextWriter &writer, Web::Css::Sst::Type val) {
        return (writer.writeStr(try$(Io::toParamCase(toStr(val)))));
    }
};
