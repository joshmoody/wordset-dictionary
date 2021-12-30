CREATE TABLE IF NOT EXISTS "words" (
    "wordset_id" varchar(10) NOT NULL,
    "word" nvarchar(100)
    );

CREATE INDEX IF NOT EXISTS WordsWordsetIdIndex ON "words" ("wordset_id" ASC);
CREATE INDEX IF NOT EXISTS WordsWordIndex ON "words" ("word" ASC);

CREATE TABLE IF NOT EXISTS "meanings" (
    "id" varchar(10) NOT NULL,
    "wordset_id" varchar(10),
    "definition" text(500),
    "example" text(500),
    "speech_part" varchar(20)
    );

CREATE INDEX IF NOT EXISTS MeaningsIdIndex ON "meanings" ("id" ASC);
CREATE INDEX IF NOT EXISTS MeaningsWordsetIdIndex ON "meanings" ("wordset_id" ASC);
CREATE INDEX IF NOT EXISTS MeaningsSpeechPartIndex ON "meanings" ("speech_part" ASC);

CREATE VIEW dictionary AS
SELECT words.wordset_id, words.word, meanings.id, meanings.definition, meanings.example, meanings.speech_part
FROM words JOIN meanings ON words.wordset_id = meanings.wordset_id;