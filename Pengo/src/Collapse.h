#pragma once

#include <memory>

#include "ast\Expression.h"
#include "ast\Statement.h"
#include "parser\Parser.h"
#include <vector>

std::vector<std::unique_ptr<Statement>> collapseParseTree(const Node* root);

static std::vector<std::unique_ptr<Statement>> deriveStatements(const Node* root);
static std::unique_ptr<Statement> deriveStatement(const Node& root);
static std::unique_ptr<Expression> deriveExpression(const Node& root);
static Token deriveToken(const Node& root);