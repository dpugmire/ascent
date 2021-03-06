#ifndef ASCENT_RUNTIME_AST
#define ASCENT_RUNTIME_AST
#include "flow_workspace.hpp"
#include <iostream>
#include <vector>

class ASTNode
{
public:
  virtual ~ASTNode()
  {
  }
  virtual void access() = 0;
  virtual conduit::Node build_graph(flow::Workspace &w) = 0;
};

class ASTExpression : public ASTNode
{
public:
  virtual void access();
  virtual conduit::Node build_graph(flow::Workspace &w);
};

class ASTIdentifier : public ASTExpression
{
public:
  std::string m_name;
  ASTIdentifier(const std::string &name) : m_name(name)
  {
  }
  virtual void access();
  virtual conduit::Node build_graph(flow::Workspace &w);
};

class ASTNamedExpression : public ASTExpression
{
public:
  ASTIdentifier *key;
  ASTExpression *value;
  ASTNamedExpression(ASTIdentifier *key, ASTExpression *value)
      : key(key), value(value)
  {
  }
  virtual void access();
  virtual conduit::Node build_graph(flow::Workspace &w);

  virtual ~ASTNamedExpression()
  {
    delete key;
    delete value;
  }
};

typedef std::vector<ASTNamedExpression *> ASTNamedExpressionList;

class ASTInteger : public ASTExpression
{
public:
  int m_value;
  ASTInteger(int value) : m_value(value)
  {
  }
  virtual void access();
  virtual conduit::Node build_graph(flow::Workspace &w);
};

class ASTDouble : public ASTExpression
{
public:
  double m_value;
  ASTDouble(double value) : m_value(value)
  {
  }
  virtual void access();
  virtual conduit::Node build_graph(flow::Workspace &w);
};

class ASTString : public ASTExpression
{
public:
  std::string m_name;
  ASTString(const std::string &name) : m_name(name)
  {
  }
  virtual void access();
  virtual conduit::Node build_graph(flow::Workspace &w);
};

class ASTBoolean : public ASTExpression
{
public:
  int tok;
  ASTBoolean(const int tok) : tok(tok)
  {
  }
  virtual void access();
  virtual conduit::Node build_graph(flow::Workspace &w);
};

class ASTExpressionList : public ASTExpression
{
public:
  std::vector<ASTExpression *> exprs;
  virtual void access();
  virtual conduit::Node build_graph(flow::Workspace &w);

  virtual ~ASTExpressionList()
  {
    for(size_t i = 0; i < exprs.size(); ++i)
    {
      delete exprs[i];
    }
  }
};

class ASTArguments
{
public:
  ASTExpressionList *pos_args;
  ASTNamedExpressionList *named_args;
  ASTArguments(ASTExpressionList *pos_args, ASTNamedExpressionList *named_args)
      : pos_args(pos_args), named_args(named_args)
  {
  }
  virtual void access();

  virtual ~ASTArguments()
  {
    delete pos_args;

    if(named_args != nullptr)
    {
      const size_t named_size = named_args->size();
      for(size_t i = 0; i < named_size; ++i)
      {
        delete(*named_args)[i];
      }
      delete named_args;
    }
  }
};

class ASTMethodCall : public ASTExpression
{
public:
  ASTIdentifier *m_id;
  ASTArguments *arguments;
  ASTMethodCall(ASTIdentifier *id, ASTArguments *arguments)
      : m_id(id), arguments(arguments)
  {
  }
  ASTMethodCall(ASTIdentifier *id) : m_id(id)
  {
  }
  virtual void access();
  virtual conduit::Node build_graph(flow::Workspace &w);

  virtual ~ASTMethodCall()
  {
    delete arguments;
    delete m_id;
  }
};

class ASTBinaryOp : public ASTExpression
{
public:
  ASTExpression *m_lhs;
  int m_op;
  ASTExpression *m_rhs;
  ASTBinaryOp(ASTExpression *lhs, int op, ASTExpression *rhs)
      : m_lhs(lhs), m_op(op), m_rhs(rhs)
  {
  }
  virtual void access();
  virtual conduit::Node build_graph(flow::Workspace &w);

  virtual ~ASTBinaryOp()
  {
    delete m_lhs;
    delete m_rhs;
  }
};

class ASTIfExpr : public ASTExpression
{
public:
  ASTExpression *m_condition;
  ASTExpression *m_if;
  ASTExpression *m_else;
  ASTIfExpr(ASTExpression *m_condition,
            ASTExpression *m_if,
            ASTExpression *m_else)
      : m_condition(m_condition), m_if(m_if), m_else(m_else)
  {
  }
  virtual void access();
  virtual conduit::Node build_graph(flow::Workspace &w);

  virtual ~ASTIfExpr()
  {
    delete m_condition;
    delete m_if;
    delete m_else;
  }
};

class ASTArrayAccess : public ASTExpression
{
public:
  ASTExpression *array;
  ASTExpression *index;
  ASTArrayAccess(ASTExpression *array, ASTExpression *index)
      : array(array), index(index)
  {
  }
  virtual void access();
  virtual conduit::Node build_graph(flow::Workspace &w);

  virtual ~ASTArrayAccess()
  {
    delete array;
    delete index;
  }
};

class ASTDotAccess : public ASTExpression
{
public:
  ASTExpression *obj;
  std::string name;
  ASTDotAccess(ASTExpression *obj, const std::string &name)
      : obj(obj), name(name)
  {
  }
  virtual void access();
  virtual conduit::Node build_graph(flow::Workspace &w);

  virtual ~ASTDotAccess()
  {
    delete obj;
  }
};
#endif
