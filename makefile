markov_files = markov_chain.c linked_list.c

# tweets:
main_tweets = tweets_generator.c

tweets_generator:
	gcc $(main_tweets) $(markov_files) -o tweets_generator

#tar_tweets_generator: # NOT NEEDED BY STUDENT
#	tar -cf ex3B.tar $(main_tweets) $(files) justdoit_tweets.txt


# snakes:
main_snakes_and_ladders = snakes_and_ladders.c

snakes_and_ladders:
	gcc $(main_snakes_and_ladders) $(markov_files) -o snakes_and_ladders

clean: # NOT NEEDED BY STUDENT
	rm -f *.o tweets_generator snakes_and_ladders

# lunch:
main_meals = meal_test.c

meal_test:
	gcc $(main_meals) $(markov_files) -o meal_test