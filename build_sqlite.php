<?php

$data_path = __DIR__ . '/data';
$database_path = __DIR__ . '/dictionary.sqlite';

$pdo = new \PDO('sqlite:' . $database_path);
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$pdo->exec("DELETE FROM words");
$pdo->exec("DELETE FROM meanings");

$word_sql = "INSERT INTO words (wordset_id, word) VALUES (:wordset_id, :word)";
$word_stmt = $pdo->prepare($word_sql);

$meaning_sql = "INSERT INTO meanings (id, wordset_id, definition, example, speech_part) VALUES (:id, :wordset_id, :definition, :example, :speech_part)";
$meaning_stmt = $pdo->prepare($meaning_sql);

$max_word_length = 0;
$max_definition_length = 0;
$word_count = 0;

foreach(glob("$data_path/*.json") as $letter){
	$source = basename($letter);

	if ($source == 'misc.json'){
		continue;
	}

	print "Processing $source" . PHP_EOL;

	$words = json_decode(file_get_contents($letter));

	foreach($words as $word){
		$word_count++;
		$word_stmt->execute([
			'wordset_id' => $word->wordset_id,
			'word' => trim($word->word)
		]);

		$word_length = strlen($word->word);

		if ($word_length > $max_word_length){
			$max_word_length = $word_length;
		}

		if (empty($word->meanings)){
			continue;
		}

		foreach($word->meanings as $meaning){
			$meaning_stmt->execute([
				'id' => $meaning->id,
				'wordset_id' => $word->wordset_id,
				'definition' => $meaning->def,
				'example' => $meaning->example ?? null,
				'speech_part' => $meaning->speech_part
			]);

			$definition_length = strlen($meaning->def);
			if ($definition_length > $max_definition_length){
				$max_definition_length = $definition_length;
			}
		}
	}
}

print "Number of Words: $word_count" . PHP_EOL;
print "Max Word Length: $max_word_length" . PHP_EOL;
print "Max Definition Length: $max_definition_length" . PHP_EOL;