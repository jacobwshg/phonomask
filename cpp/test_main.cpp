#include "test.h"
#include "utils.h"
#include "feat_idx_maps.h"
#include "feature_profile.h"
#include "rule.h"
#include "masks.h"
#include "word.h"
#include <iostream>
//#include <fstream>
#include <vector>
#include <string>
#include <string_view>


void
header_test( std::istream &table_stream )
{
	table_stream.seekg(0);
	std::vector<std::string> hdr_fields 
	{
		Phmask::fields_from_row( table_stream )
	};

	Phmask::FeatIdxMaps idxmaps {};
	idxmaps.populate( hdr_fields );

	std::cout << idxmaps.str() << "\n";

	std::string lay {idxmaps.feature_layout_str()};
	std::cout << lay << "\n";
}

int 
main(
	[[maybe_unused]] int argc, 
	[[maybe_unused]] char *argv[]
)
{
	std::cout << "\nWord segmentation test\n";
	std::string word {" pʰɪ̃n̪t͡ʃ "};
	std::cout << word << "\n";
	std::vector<std::string> segments { Phmask::word_to_segments( word ) };
	for ( const std::string &s : segments )
	{
		std::cout << s << "\n";
	}

///*
	if ( argc<2 )
	{
		std::cout<<"Usage: phmask <table path>\n";
		return 2;
	}

	std::string path { argv[1] };
	Phmask::FeatureProfile profile { path };

	std::cout 
		<< "Segment-feature matrix map \n"
		<< profile.seg_fm_maps.str()
		<< "\n";

	std::cout << "feature layout str:\n"
		<< profile.feat_idx_maps.feature_layout_str()
		<< "\n";

	const std::vector<std::string> test_segs { "v", "d", "a",  "t", "b"  };
	for ( const std::string &ts:test_segs )
	{
		std::cout << "["<<ts<<"] all features str: "
			<< profile.seg_feat_mtx_str( ts )<<"\n";
	}

	word = "atparg";
	std::cout << "Creating word representation\n";
	Phmask::WordRepr wr { profile.wordrepr_from_str( word ) };
	std::cout << "Printing word from word representation\n";
	std::cout << profile.wordrepr_to_str( wr ) << "\n";

	const std::string interC_insert_a
	{
		"∅ -> a / [+cons] _ [+cons,]	 "
	};
	const std::string &rule1 { interC_insert_a };

	const std::string interV_C_voi
	{
		"[+cons, -syl, -son, -voi] -> [+voi] / [-cons, +syl, +son, +voi] _ [-cons, +syl, +son, +voi]"
	};
	const std::string &rule2 { interV_C_voi };

	const std::string final_noncont_test_rule
	{
		" [-cont, +cons, ] -> ∅ / _ # "
	};
	const std::string &rule3 { final_noncont_test_rule };

	std::string _funny { "p -> b /	 " };

	std::cout << "Rule strings: \n" 
		<< rule1 <<"\n" 
		<< rule2 << "\n"
		<< rule3 << "\n";
	const Phmask::Rule
		r1 { profile.rule_from_str( rule1 ) },
		r2 { profile.rule_from_str( rule2 ) },
		r3 { profile.rule_from_str( rule3 ) };

	std::cout<<"************************************************\n";
	std::cout << "rule " << rule1 << " masks str: \n"
		<< r1.masks_str() << "\n";
	std::cout << "rule " << rule2 << " masks str: \n"
		<< r2.masks_str() << "\n";
	std::cout << "rule " << rule3 << " masks str: \n"
		<< r3.masks_str() << "\n";
	std::cout<<"************************************************\n";

	wr.apply_rule( r1 );
	std::cout << "Word after rule application: " << profile.wordrepr_to_str( wr ) << "\n";
	wr.apply_rule( r2 );
	std::cout << "Word after rule application: " << profile.wordrepr_to_str( wr ) << "\n";
	wr.apply_rule( r3 );
	std::cout << "Word after rule application: " << profile.wordrepr_to_str( wr ) << "\n";

	//const Phmask::Rule r_cons_devoi { profile.rule_from_str( std::string { "[+cons] -> [-voi]" } ) };
	//Phmask::WordRepr wr3 { profile.wordrepr_from_str( std::string{ "fhkeffective_featsgeabu" } ) };
	//wr3.apply_rule( r_cons_devoi );
	//std::cout << "wr3 after rule application: "<<profile.wordrepr_to_str( wr3 )<<"\n";

//*/
	return 0;
}

