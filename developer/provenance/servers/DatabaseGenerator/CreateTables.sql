-- TrustedServer tables

-- With INTEGER PRIMARY KEY, the DocumentIDs are automatically generated
CREATE TABLE [Document] (
  [DocumentID] INTEGER PRIMARY KEY, 
  [Title] TEXT,
  [CreatedBy] INTEGER,
  [CreatedOn] TEXT,
  [LatestVersionID] TEXT,
  [StatusID] INTEGER,
  [IsPlaceHolder] BOOL,
  [AllocatedOn] TEXT, 
  [UniqueKey] TEXT, 
  [Version] INT64
);

CREATE TABLE [UserDocumentMapping] (
  [UserID] INTEGER NOT NULL, 
  [DocumentID] INTEGER NOT NULL, 
  [IsUser] BOOL NOT NULL, 
  [IsAuditor] BOOL NOT NULL, 
  [IsAdministrator] BOOL NOT NULL
);

CREATE TABLE [User] (
	[UserID] INTEGER PRIMARY KEY,
	[FirstName] TEXT NOT NULL,
	[MiddleName] TEXT,
	[LastName] TEXT NOT NULL,
	[EmailAddress] TEXT NOT NULL,
	[Password] TEXT NOT NULL,
	[ssl_key] BLOB NOT NULL,
	[ssl_cert] BLOB NOT NULL,
	[private_key] BLOB NOT NULL,
	[public_key] BLOB NOT NULL,
	[IsActive] SMALLINT DEFAULT 1,
	[UserType] SMALLINT DEFAULT 1
);

