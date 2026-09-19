#pragma once

namespace tool::gsc::ast {
    enum AstType {
        //
    };

    class AstNode {
        AstType type;

      public:
        AstNode(AstType type) : type(type) {}
        virtual ~AstNode() = default;

        constexpr AstType GetType() const { return type; }
    };

} // namespace tool::gsc::ast