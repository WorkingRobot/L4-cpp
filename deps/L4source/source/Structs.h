#pragma once

#include <cstdint>

namespace L4::Source
{
    enum class InterfaceVersion : uint16_t
    {
        Unknown = 0,
        Initial,

        LatestPlusOne,
        Latest = LatestPlusOne - 1
    };

    struct String
    {
        const char8_t* Data;
        uint64_t Size;
    };

    struct Version
    {
        String Humanized;
        uint32_t Numeric;
    };

    struct BaseIdentity
    {
        String Id;
        String Name;
        Version Version;
    };

    typedef BaseIdentity SourceIdentity;

    typedef BaseIdentity L4Identity;

    struct AppIdentity
    {
        BaseIdentity App;
        SourceIdentity Source;
        String Environment;
    };

    struct StreamIdentity
    {
        String Id;
        String Name;
        uint32_t Version;
    };

    typedef void* BaseContext;

    typedef BaseContext Archive;

    typedef BaseContext Update;

    typedef BaseContext Stream;

    enum class UpdateState : uint8_t
    {
        Unknown,
        Starting,
        Running,
        Paused,
        Finalizing,
        Complete
    };

    struct UpdateStartInfo
    {
        uint64_t TotalPieceCount;
        uint64_t TotalDownloadByteCount;
        uint64_t TotalReadByteCount;
        uint64_t TotalWriteByteCount;
    };

    struct UpdateProgressInfo
    {
        uint64_t TotalPieceCount;
        uint64_t CurrentPieceCount;

        uint64_t TotalDownloadByteCount;
        uint64_t CurrentDownloadByteCount;
        uint64_t DownloadByteRate;

        uint64_t TotalReadByteCount;
        uint64_t CurrentReadByteCount;
        uint64_t ReadByteRate;

        uint64_t TotalWriteByteCount;
        uint64_t CurrentWriteByteCount;
        uint64_t WriteByteRate;
    };

    enum class UpdatePieceStatus : uint8_t
    {
        Unknown,
        Scheduled,
        Initializing,
        Transferring,
        Downloading,
        WritingMetadata,
        WritingData,
        Completed
    };

    struct UserIdentity
    {
        String Id;
        String DisplayName;
    };

    enum class AuthMethodType : uint8_t
    {
        Form,
        WebCallback,
        AuthCode
    };

    enum class AuthMethodFormFieldType : uint8_t
    {
        Text,
        Checkbox,
        Dropdown,
        SubmitButton
    };

    struct AuthMethodFormFieldDropdownEntry
    {
        String Id;
        String Name;
    };

    struct AuthMethodFormField
    {
        AuthMethodFormFieldType Type;
        String Id;
        String Name;
        union
        {
            struct
            {
                String Placeholder;
                String VerifyRegex;
                String VerifyErrorMessage;
                bool IsPassword;
            } Text;
            struct
            {
                bool CheckedByDefault;
            } Checkbox;
            struct
            {
                const AuthMethodFormFieldDropdownEntry* Entries;
                uint32_t EntryCount;
                String DefaultEntry;
                bool IsRadio;
            } Dropdown;
            struct
            {

            } SubmitButton;
        };
    };

    struct AuthMethod
    {
        AuthMethodType Type;
        String Id;
        String DisplayName;
        String Icon;
        union
        {
            struct
            {
                const AuthMethodFormField* Fields;
                uint32_t FieldCount;
            } Form;
            struct
            {

            } WebCallback;
            struct
            {
                String Description;
                String Placeholder;
                String VerifyRegex;
                String VerifyErrorMessage;
            } AuthCode;
        };
    };

    struct AuthMethodFulfilledFormField
    {
        String Id;
        union
        {
            String Text;
            bool Checkbox;
            String Dropdown;
            bool SubmitButton;
        };
    };

    struct AuthMethodFulfilled
    {
        const AuthMethod* Method;
        union
        {
            struct
            {
                const AuthMethodFulfilledFormField* Fields;
                uint32_t FieldCount;
            } Form;
            struct
            {
            
            } WebCallback;
            struct
            {
                String Code;
            } AuthCode;
        };
    };

    struct SourceInterface
    {
        SourceIdentity Identity;

        void (*ConfigOpen)(String ConfigDirectory);

        void (*GetUserIdentity)(UserIdentity* OutIdentity);

        uint32_t (*GetInitialAuthMethods)(const AuthMethod** AuthMethods);

        bool (*FulfillAuthMethod)(const AuthMethodFulfilled* FulfilledAuthMethod, AuthMethod* NextAuthMethod);

        uint32_t (*GetAvailableApps)(const AppIdentity** Apps);

        bool (*IsValidApp)(const AppIdentity* Identity);

        // L4 passes a new Update object, the source grabs the new current version and attaches some context to keep track of it
        void (*UpdateOpen)(Update Update, const AppIdentity* OldIdentity, AppIdentity* NewIdentity);

        // Stop the update if started, close the Update object, it won't be referenced again (the source should dispose of its internal context data and its archive if it's not nullptr)
        void (*UpdateClose)(Update Update);

        // Begin an actual update sequence, archive object is valid, source can attach its context to it
        void (*UpdateStart)(Update Update, Archive Archive, uint32_t ProgressUpdateRateMs);

        // Pause the update sequence. Can call UpdateResume to resume the update
        void (*UpdatePause)(Update Update);

        // Resume the update sequence. Called after UpdatePause
        void (*UpdateResume)(Update Update);
    };

    struct L4Interface
    {
        L4Identity Identity;

        // Archive
        void (*ArchiveGetIdentity)(Archive Archive, AppIdentity* OutIdentity);
        void (*ArchiveSetIdentity)(Archive Archive, const AppIdentity* NewIdentity);
        uint32_t (*ArchiveGetStreamCount)(Archive Archive);
        uint32_t (*ArchiveGetSectorSize)(Archive Archive);
        uint32_t (*ArchiveGetStreamIdxFromId)(Archive Archive, const String* Id);
        void (*ArchiveOpenStreamRead)(Archive Archive, uint32_t StreamIdx, Stream* OutStream);
        void (*ArchiveOpenStreamWrite)(Archive Archive, uint32_t StreamIdx, Stream* OutStream);
        void (*ArchiveCloseStream)(Archive Archive, Stream Stream);

        // Stream
        void (*ArchiveStreamGetIdentity)(Stream Stream, StreamIdentity* OutIdentity);
        void (*ArchiveStreamSetIdentity)(Stream Stream, const StreamIdentity* NewIdentity);
        uint32_t (*ArchiveStreamGetElementSize)(Stream Stream);
        void (*ArchiveStreamSetElementSize)(Stream Stream, uint32_t NewElementSize);
        void (*ArchiveStreamReadContext)(Stream Stream, void* Dst, uint32_t Size);
        void (*ArchiveStreamWriteContext)(Stream Stream, const void* Src, uint32_t Size);
        uint64_t (*ArchiveStreamGetCapacity)(Stream Stream);
        uint64_t (*ArchiveStreamGetSize)(Stream Stream);
        void (*ArchiveStreamReserveCapacity)(Stream Stream, uint64_t NewCapacity);
        void (*ArchiveStreamShrinkToFit)(Stream Stream);
        void (*ArchiveStreamResize)(Stream Stream, uint64_t NewSize);
        uint64_t (*ArchiveStreamReadBytes)(Stream Stream, void* Dst, uint64_t Size, uint64_t Offset);
        uint64_t (*ArchiveStreamWriteBytes)(Stream Stream, const void* Src, uint64_t Size, uint64_t Offset);

        // Update
        UpdateState (*UpdateGetState)(Update Update);
        void (*UpdateOnStart)(Update Update, const UpdateStartInfo* StartInfo);
        void (*UpdateOnProgress)(Update Update, const UpdateProgressInfo* ProgressInfo);
        void (*UpdateOnPieceUpdate)(Update Update, uint64_t Id, UpdatePieceStatus NewStatus);
        void (*UpdateOnFinalize)(Update Update);
        void (*UpdateOnComplete)(Update Update);
    };
}
