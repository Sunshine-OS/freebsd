//===-- Function.h ----------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef liblldb_Function_h_
#define liblldb_Function_h_

#include "lldb/Core/ClangForward.h"
#include "lldb/Core/AddressRange.h"
#include "lldb/Symbol/Block.h"
#include "lldb/Symbol/Declaration.h"
#include "lldb/Expression/DWARFExpression.h"
#include "lldb/Core/Mangled.h"
#include "lldb/Core/UserID.h"

namespace lldb_private {

//----------------------------------------------------------------------
/// @class FunctionInfo Function.h "lldb/Symbol/Function.h"
/// @brief A class that contains generic function information.
///
/// This provides generic function information that gets resused between
/// inline functions and function types.
//----------------------------------------------------------------------
class FunctionInfo
{
public:
    //------------------------------------------------------------------
    /// Construct with the function method name and optional declaration
    /// information.
    ///
    /// @param[in] name
    ///     A C string name for the method name for this function. This
    ///     value should not be the mangled named, but the simple method
    ///     name.
    ///
    /// @param[in] decl_ptr
    ///     Optional declaration information that describes where the
    ///     function was declared. This can be NULL.
    //------------------------------------------------------------------
    FunctionInfo (const char *name, const Declaration *decl_ptr);

    //------------------------------------------------------------------
    /// Construct with the function method name and optional declaration
    /// information.
    ///
    /// @param[in] name
    ///     A name for the method name for this function. This value
    ///     should not be the mangled named, but the simple method name.
    ///
    /// @param[in] decl_ptr
    ///     Optional declaration information that describes where the
    ///     function was declared. This can be NULL.
    //------------------------------------------------------------------
    FunctionInfo (const ConstString& name, const Declaration *decl_ptr);

    //------------------------------------------------------------------
    /// Destructor.
    ///
    /// The destructor is virtual since classes inherit from this class.
    //------------------------------------------------------------------
    virtual
    ~FunctionInfo ();

    //------------------------------------------------------------------
    /// Compare two function information objects.
    ///
    /// First compares the method names, and if equal, then compares
    /// the declaration information.
    ///
    /// @param[in] lhs
    ///     The Left Hand Side const FunctionInfo object reference.
    ///
    /// @param[in] rhs
    ///     The Right Hand Side const FunctionInfo object reference.
    ///
    /// @return
    ///     @li -1 if lhs < rhs
    ///     @li 0 if lhs == rhs
    ///     @li 1 if lhs > rhs
    //------------------------------------------------------------------
    static int
    Compare (const FunctionInfo& lhs, const FunctionInfo& rhs);

    //------------------------------------------------------------------
    /// Dump a description of this object to a Stream.
    ///
    /// Dump a description of the contents of this object to the
    /// supplied stream \a s.
    ///
    /// @param[in] s
    ///     The stream to which to dump the object descripton.
    //------------------------------------------------------------------
    void
    Dump (Stream *s, bool show_fullpaths) const;

    //------------------------------------------------------------------
    /// Get accessor for the declaration information.
    ///
    /// @return
    ///     A reference to the declaration object.
    //------------------------------------------------------------------
    Declaration&
    GetDeclaration ();

    //------------------------------------------------------------------
    /// Get const accessor for the declaration information.
    ///
    /// @return
    ///     A const reference to the declaration object.
    //------------------------------------------------------------------
    const Declaration&
    GetDeclaration () const;

    //------------------------------------------------------------------
    /// Get accessor for the method name.
    ///
    /// @return
    ///     A const reference to the method name object.
    //------------------------------------------------------------------
    const ConstString&
    GetName () const;

    //------------------------------------------------------------------
    /// Get the memory cost of this object.
    ///
    /// @return
    ///     The number of bytes that this object occupies in memory.
    ///     The returned value does not include the bytes for any
    ///     shared string values.
    ///
    /// @see ConstString::StaticMemorySize ()
    //------------------------------------------------------------------
    virtual size_t
    MemorySize () const;

protected:
    //------------------------------------------------------------------
    // Member variables.
    //------------------------------------------------------------------
    ConstString m_name; ///< Function method name (not a mangled name).
    Declaration m_declaration; ///< Information describing where this function information was defined.
};


//----------------------------------------------------------------------
/// @class InlineFunctionInfo Function.h "lldb/Symbol/Function.h"
/// @brief A class that describes information for an inlined function.
//----------------------------------------------------------------------
class InlineFunctionInfo : public FunctionInfo
{
public:
    //------------------------------------------------------------------
    /// Construct with the function method name, mangled name, and
    /// optional declaration information.
    ///
    /// @param[in] name
    ///     A C string name for the method name for this function. This
    ///     value should not be the mangled named, but the simple method
    ///     name.
    ///
    /// @param[in] mangled
    ///     A C string name for the mangled name for this function. This
    ///     value can be NULL if there is no mangled information.
    ///
    /// @param[in] decl_ptr
    ///     Optional declaration information that describes where the
    ///     function was declared. This can be NULL.
    ///
    /// @param[in] call_decl_ptr
    ///     Optional calling location declaration information that
    ///     describes from where this inlined function was called.
    //------------------------------------------------------------------
    InlineFunctionInfo(const char *name, const char *mangled, const Declaration *decl_ptr, const Declaration *call_decl_ptr);

    //------------------------------------------------------------------
    /// Construct with the function method name, mangled name, and
    /// optional declaration information.
    ///
    /// @param[in] name
    ///     A name for the method name for this function. This value
    ///     should not be the mangled named, but the simple method name.
    ///
    /// @param[in] mangled
    ///     A name for the mangled name for this function. This value
    ///     can be empty if there is no mangled information.
    ///
    /// @param[in] decl_ptr
    ///     Optional declaration information that describes where the
    ///     function was declared. This can be NULL.
    ///
    /// @param[in] call_decl_ptr
    ///     Optional calling location declaration information that
    ///     describes from where this inlined function was called.
    //------------------------------------------------------------------
    InlineFunctionInfo(const ConstString& name, const Mangled &mangled, const Declaration *decl_ptr, const Declaration *call_decl_ptr);

    //------------------------------------------------------------------
    /// Destructor.
    //------------------------------------------------------------------
    ~InlineFunctionInfo();

    //------------------------------------------------------------------
    /// Compare two inlined function information objects.
    ///
    /// First compares the FunctionInfo objects, and if equal,
    /// compares the mangled names.
    ///
    /// @param[in] lhs
    ///     The Left Hand Side const InlineFunctionInfo object
    ///     reference.
    ///
    /// @param[in] rhs
    ///     The Right Hand Side const InlineFunctionInfo object
    ///     reference.
    ///
    /// @return
    ///     @li -1 if lhs < rhs
    ///     @li 0 if lhs == rhs
    ///     @li 1 if lhs > rhs
    //------------------------------------------------------------------
    int
    Compare(const InlineFunctionInfo& lhs, const InlineFunctionInfo& rhs);

    //------------------------------------------------------------------
    /// Dump a description of this object to a Stream.
    ///
    /// Dump a description of the contents of this object to the
    /// supplied stream \a s.
    ///
    /// @param[in] s
    ///     The stream to which to dump the object descripton.
    //------------------------------------------------------------------
    void
    Dump(Stream *s, bool show_fullpaths) const;

    void
    DumpStopContext (Stream *s) const;

    const ConstString &
    GetName () const;

    //------------------------------------------------------------------
    /// Get accessor for the call site declaration information.
    ///
    /// @return
    ///     A reference to the declaration object.
    //------------------------------------------------------------------
    Declaration&
    GetCallSite ();

    //------------------------------------------------------------------
    /// Get const accessor for the call site declaration information.
    ///
    /// @return
    ///     A const reference to the declaration object.
    //------------------------------------------------------------------
    const Declaration&
    GetCallSite () const;

    //------------------------------------------------------------------
    /// Get accessor for the mangled name object.
    ///
    /// @return
    ///     A reference to the mangled name object.
    //------------------------------------------------------------------
    Mangled&
    GetMangled();

    //------------------------------------------------------------------
    /// Get const accessor for the mangled name object.
    ///
    /// @return
    ///     A const reference to the mangled name object.
    //------------------------------------------------------------------
    const Mangled&
    GetMangled() const;

    //------------------------------------------------------------------
    /// Get the memory cost of this object.
    ///
    /// @return
    ///     The number of bytes that this object occupies in memory.
    ///     The returned value does not include the bytes for any
    ///     shared string values.
    ///
    /// @see ConstString::StaticMemorySize ()
    //------------------------------------------------------------------
    virtual size_t
    MemorySize() const;

private:
    //------------------------------------------------------------------
    // Member variables.
    //------------------------------------------------------------------
    Mangled m_mangled; ///< Mangled inlined function name (can be empty if there is no mangled information).
    Declaration m_call_decl;
};

//----------------------------------------------------------------------
/// @class Function Function.h "lldb/Symbol/Function.h"
/// @brief A class that describes a function.
///
/// Functions belong to CompileUnit objects (Function::m_comp_unit),
/// have unique user IDs (Function::UserID), know how to reconstruct
/// their symbol context (Function::SymbolContextScope), have a
/// specific function type (Function::m_type_uid), have a simple
/// method name (FunctionInfo::m_name), be declared at a specific
/// location (FunctionInfo::m_declaration), possibly have mangled
/// names (Function::m_mangled), an optional return type
/// (Function::m_type), and contains lexical blocks
/// (Function::m_blocks).
///
/// The function inforation is split into a few pieces:
///     @li The concrete instance information
///     @li The abstract information
///
/// The abstract information is found in the function type (Type) that
/// describes a function information, return type and parameter types.
///
/// The concreate information is the address range information and
/// specific locations for an instance of this function.
//----------------------------------------------------------------------
class Function :
    public UserID,
    public SymbolContextScope
{
public:
    //------------------------------------------------------------------
    /// Construct with a compile unit, function UID, function type UID,
    /// optional mangled name, function type, and a section offset
    /// based address range.
    ///
    /// @param[in] comp_unit
    ///     The compile unit to which this function belongs.
    ///
    /// @param[in] func_uid
    ///     The UID for this function. This value is provided by the
    ///     SymbolFile plug-in and can be any value that allows
    ///     the plug-in to quickly find and parse more detailed
    ///     information when and if more information is needed.
    ///
    /// @param[in] func_type_uid
    ///     The type UID for the function Type to allow for lazy type
    ///     parsing from the debug information.
    ///
    /// @param[in] mangled
    ///     The optional mangled name for this function. If empty, there
    ///     is no mangled information.
    ///
    /// @param[in] func_type
    ///     The optional function type. If NULL, the function type will
    ///     be parsed on demand when accessed using the
    ///     Function::GetType() function by asking the SymbolFile
    ///     plug-in to get the type for \a func_type_uid.
    ///
    /// @param[in] range
    ///     The section offset based address for this function.
    //------------------------------------------------------------------
    Function (
        CompileUnit *comp_unit,
        lldb::user_id_t func_uid,
        lldb::user_id_t func_type_uid,
        const Mangled &mangled,
        Type * func_type,
        const AddressRange& range);

    //------------------------------------------------------------------
    /// Construct with a compile unit, function UID, function type UID,
    /// optional mangled name, function type, and a section offset
    /// based address range.
    ///
    /// @param[in] comp_unit
    ///     The compile unit to which this function belongs.
    ///
    /// @param[in] func_uid
    ///     The UID for this function. This value is provided by the
    ///     SymbolFile plug-in and can be any value that allows
    ///     the plug-in to quickly find and parse more detailed
    ///     information when and if more information is needed.
    ///
    /// @param[in] func_type_uid
    ///     The type UID for the function Type to allow for lazy type
    ///     parsing from the debug information.
    ///
    /// @param[in] mangled
    ///     The optional mangled name for this function. If empty, there
    ///     is no mangled information.
    ///
    /// @param[in] func_type
    ///     The optional function type. If NULL, the function type will
    ///     be parsed on demand when accessed using the
    ///     Function::GetType() function by asking the SymbolFile
    ///     plug-in to get the type for \a func_type_uid.
    ///
    /// @param[in] range
    ///     The section offset based address for this function.
    //------------------------------------------------------------------
    Function (
        CompileUnit *comp_unit,
        lldb::user_id_t func_uid,
        lldb::user_id_t func_type_uid,
        const char *mangled,
        Type * func_type,
        const AddressRange& range);

    //------------------------------------------------------------------
    /// Destructor.
    //------------------------------------------------------------------
    ~Function ();

    //------------------------------------------------------------------
    /// @copydoc SymbolContextScope::CalculateSymbolContext(SymbolContext*)
    ///
    /// @see SymbolContextScope
    //------------------------------------------------------------------
    virtual void
    CalculateSymbolContext(SymbolContext* sc);

    virtual lldb::ModuleSP
    CalculateSymbolContextModule ();
    
    virtual CompileUnit *
    CalculateSymbolContextCompileUnit ();
    
    virtual Function *
    CalculateSymbolContextFunction ();

    const AddressRange &
    GetAddressRange()
    {
        return m_range;
    }

    //------------------------------------------------------------------
    /// Find the file and line number of the source location of the start
    /// of the function.  This will use the declaration if present and fall
    /// back on the line table if that fails.  So there may NOT be a line
    /// table entry for this source file/line combo.
    ///
    /// @param[out] source_file
    ///     The source file.
    ///
    /// @param[out] line_no
    ///     The line number.
    //------------------------------------------------------------------
    void
    GetStartLineSourceInfo (FileSpec &source_file, uint32_t &line_no);
    
     //------------------------------------------------------------------
    /// Find the file and line number of the source location of the end
    /// of the function.
    ///
    ///
    /// @param[out] source_file
    ///     The source file.
    ///
    /// @param[out] line_no
    ///     The line number.
    //------------------------------------------------------------------
    void
    GetEndLineSourceInfo (FileSpec &source_file, uint32_t &line_no);

    //------------------------------------------------------------------
    /// Get accessor for the block list.
    ///
    /// @return
    ///     The block list object that describes all lexical blocks
    ///     in the function.
    ///
    /// @see BlockList
    //------------------------------------------------------------------
    Block&
    GetBlock (bool can_create);

    //------------------------------------------------------------------
    /// Get accessor for the compile unit that owns this function.
    ///
    /// @return
    ///     A compile unit object pointer.
    //------------------------------------------------------------------
    CompileUnit*
    GetCompileUnit();

    //------------------------------------------------------------------
    /// Get const accessor for the compile unit that owns this function.
    ///
    /// @return
    ///     A const compile unit object pointer.
    //------------------------------------------------------------------
    const CompileUnit*
    GetCompileUnit() const;

    void
    GetDescription(Stream *s, lldb::DescriptionLevel level, Target *target);

    //------------------------------------------------------------------
    /// Get accessor for the frame base location.
    ///
    /// @return
    ///     A location expression that describes the function frame
    ///     base.
    //------------------------------------------------------------------
    DWARFExpression &
    GetFrameBaseExpression()
    {
        return m_frame_base;
    }

    //------------------------------------------------------------------
    /// Get const accessor for the frame base location.
    ///
    /// @return
    ///     A const compile unit object pointer.
    //------------------------------------------------------------------
    const DWARFExpression &
    GetFrameBaseExpression() const
    {
        return m_frame_base;
    }

    const ConstString &
    GetName() const
    {
        return m_mangled.GetName();
    }

    const Mangled &
    GetMangled() const
    {
        return m_mangled;
    }

    //------------------------------------------------------------------
    /// Get the DeclContext for this function, if available.
    ///
    /// @return
    ///     The DeclContext, or NULL if none exists.
    //------------------------------------------------------------------
    clang::DeclContext *
    GetClangDeclContext();
    
    //------------------------------------------------------------------
    /// Get accessor for the type that describes the function
    /// return value type, and paramter types.
    ///
    /// @return
    ///     A type object pointer.
    //------------------------------------------------------------------
    Type*
    GetType();

    //------------------------------------------------------------------
    /// Get const accessor for the type that describes the function
    /// return value type, and paramter types.
    ///
    /// @return
    ///     A const type object pointer.
    //------------------------------------------------------------------
    const Type*
    GetType() const;
    
    ClangASTType
    GetClangType ();

    uint32_t
    GetPrologueByteSize ();

    //------------------------------------------------------------------
    /// Dump a description of this object to a Stream.
    ///
    /// Dump a description of the contents of this object to the
    /// supplied stream \a s.
    ///
    /// @param[in] s
    ///     The stream to which to dump the object descripton.
    ///
    /// @param[in] show_context
    ///     If \b true, variables will dump their symbol context
    ///     information.
    //------------------------------------------------------------------
    void
    Dump(Stream *s, bool show_context) const;

    //------------------------------------------------------------------
    /// @copydoc SymbolContextScope::DumpSymbolContext(Stream*)
    ///
    /// @see SymbolContextScope
    //------------------------------------------------------------------
    virtual void
    DumpSymbolContext(Stream *s);

    //------------------------------------------------------------------
    /// Get the memory cost of this object.
    ///
    /// @return
    ///     The number of bytes that this object occupies in memory.
    ///     The returned value does not include the bytes for any
    ///     shared string values.
    ///
    /// @see ConstString::StaticMemorySize ()
    //------------------------------------------------------------------
    size_t
    MemorySize () const;

    lldb::DisassemblerSP
    GetInstructions (const ExecutionContext &exe_ctx,
                     const char *flavor,
                     bool prefer_file_cache);

    bool
    GetDisassembly (const ExecutionContext &exe_ctx,
                    const char *flavor,
                    bool prefer_file_cache,
                    Stream &strm);

protected:

    enum
    {
        flagsCalculatedPrologueSize = (1 << 0)  ///< Have we already tried to calculate the prologue size?
    };



    //------------------------------------------------------------------
    // Member variables.
    //------------------------------------------------------------------
    CompileUnit *m_comp_unit;       ///< The compile unit that owns this function.
    lldb::user_id_t m_type_uid;     ///< The user ID of for the prototype Type for this function.
    Type * m_type;                  ///< The function prototype type for this function that include the function info (FunctionInfo), return type and parameters.
    Mangled m_mangled;              ///< The mangled function name if any, if empty, there is no mangled information.
    Block m_block;                  ///< All lexical blocks contained in this function.
    AddressRange m_range;           ///< The function address range that covers the widest range needed to contain all blocks
    DWARFExpression m_frame_base;   ///< The frame base expression for variables that are relative to the frame pointer.
    Flags m_flags;
    uint32_t m_prologue_byte_size;  ///< Compute the prologue size once and cache it
private:
    DISALLOW_COPY_AND_ASSIGN(Function);
};

} // namespace lldb_private

#endif  // liblldb_Function_h_
