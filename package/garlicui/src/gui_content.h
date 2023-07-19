#ifndef GUI_CONTENT_H
#define GUI_CONTENT_H

/**
 * @brief Activates the currently selected content.
 */
static void gui_activate_content(struct gui_node * this)
{
	// Cast the content specific data
	struct gui_path_menu_node_data * data = (struct gui_path_menu_node_data *)this->data;

	// Get the root node of the configuration XML
	xmlNodePtr rootNode = xmlDocGetRootElement(data->configuration);

	// Traverse the child nodes
	for (xmlNodePtr node = rootNode->children; node != NULL; node = node->next)
	{
		// The current node is an XML element
		if (node->type == XML_ELEMENT_NODE)
		{
			// We've found the node that contains the runner
			if (xmlStrcmp(node->name, (const xmlChar *)FOLDER_CONFIGURATION_RUN_ELEMENT_NAME) == 0)
			{
				// Get the runner
				xmlChar * runner = xmlNodeGetContent(node);

				// We managed to get the runner
				if (runner != NULL)
				{
					// Allocate a buffer for the full core path
					char * core_path = calloc(1, strlen(RETROARCH_DEFAULT_CORE_FILE_PATH) - 1 + strlen(runner));

					// We managed to allocate a buffer for the full core path
					if (core_path != NULL)
					{
						// Puzzle together the full core path
						sprintf(core_path, RETROARCH_DEFAULT_CORE_FILE_PATH, runner);

						// Write the context configuration to disk (so we can restore the UI state on game exit)
						gui_write_configuration(this->context);

						// We've found a retroarch core with this name
						if (access(core_path, F_OK) == 0)
						{
							// Run retroarch
							retroarch_run(this->context, core_path, data->path);
						}

						// We've got no retroarch core with this name
						else
						{
							// Puzzle together the process arguments
							const char * args[] = { runner, data->path, NULL };

							// Try running the application directly
							process_run(this->context, args, 1);
						}

						// Free the full core path
						free(core_path);
					}

					// Free the core name
					xmlFree(runner);
				}
			}
		}
	}
}

#endif