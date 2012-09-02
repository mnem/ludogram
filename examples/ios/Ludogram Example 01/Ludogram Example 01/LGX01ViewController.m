//
//  LGX01ViewController.m
//  Ludogram Example 01
//
//  Created by David Wagner on 02/09/2012.
//  Copyright (c) 2012 David Wagner. All rights reserved.
//

#import "LGX01ViewController.h"

@interface LGX01ViewController ()

@end

@implementation LGX01ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
	    return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
	} else {
	    return YES;
	}
}

@end
